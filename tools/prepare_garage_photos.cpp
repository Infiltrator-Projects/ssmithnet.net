// SPDX-License-Identifier: GPL-3.0-or-later
// Native C++17 gallery preparation. Archived JPEG inputs remain read-only.
#include <cstddef>
#include <cstdio>
#include <jpeglib.h>
#include <webp/decode.h>
#include <webp/encode.h>
#include <algorithm>
#include <cmath>
#include <csetjmp>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <unistd.h>

namespace fs=std::filesystem;
struct Image {
    int width=0,height=0;
    std::vector<std::uint8_t> pixels;
    Image()=default;
    Image(int w,int h):width(w),height(h) {
        if(w<=0||h<=0||w>16384||h>16384||std::uint64_t(w)*h>64000000ULL)
            throw std::runtime_error("Invalid photograph dimensions");
        pixels.resize(std::size_t(w)*h*3);
    }
    std::uint8_t* at(int x,int y){return pixels.data()+(std::size_t(y)*width+x)*3;}
    const std::uint8_t* at(int x,int y)const{return pixels.data()+(std::size_t(y)*width+x)*3;}
};
struct JpegError { jpeg_error_mgr manager; jmp_buf jump; };
static void jpeg_fail(j_common_ptr c) {
    longjmp(reinterpret_cast<JpegError*>(c->err)->jump,1);
}
static std::uint16_t read16(const std::uint8_t* p,bool le) {
    return le?std::uint16_t(p[0]|p[1]<<8):std::uint16_t(p[0]<<8|p[1]);
}
static std::uint32_t read32(const std::uint8_t* p,bool le) {
    return le?std::uint32_t(p[0])|(std::uint32_t(p[1])<<8)|
              (std::uint32_t(p[2])<<16)|(std::uint32_t(p[3])<<24)
             :(std::uint32_t(p[0])<<24)|(std::uint32_t(p[1])<<16)|
              (std::uint32_t(p[2])<<8)|std::uint32_t(p[3]);
}
// Honour the JPEG's EXIF orientation before cropping or colour adjustment.
static int orientation(const jpeg_decompress_struct& j) {
    for(jpeg_saved_marker_ptr m=j.marker_list;m;m=m->next) {
        const auto* p=m->data;
        const std::size_t length=m->data_length;
        if(m->marker!=JPEG_APP0+1||length<20||std::memcmp(p,"Exif\0\0",6))continue;
        const auto* t=p+6;
        const std::size_t n=length-6;
        if(n<8||!((t[0]=='I'&&t[1]=='I')||(t[0]=='M'&&t[1]=='M')))continue;
        const bool le=t[0]=='I';
        if(read16(t+2,le)!=42)continue;
        const auto off=read32(t+4,le);
        if(off>n||n-off<2)continue;
        const auto count=read16(t+off,le);
        for(unsigned i=0;i<count;i++) {
            const std::size_t pos=std::size_t(off)+2+std::size_t(i)*12;
            if(pos>n||n-pos<12)break;
            if(read16(t+pos,le)==0x112&&read16(t+pos+2,le)==3&&
               read32(t+pos+4,le)==1) {
                const int o=read16(t+pos+8,le);
                if(o>=1&&o<=8)return o;
            }
        }
    }
    return 1;
}
static Image decode_jpeg(const fs::path& source,int& exif) {
    FILE* in=std::fopen(source.c_str(),"rb");
    if(!in)throw std::runtime_error("Missing archival JPEG: "+source.string());
    jpeg_decompress_struct j{};
    JpegError e{};
    Image image; // Created before setjmp; survives error cleanup.
    j.err=jpeg_std_error(&e.manager);
    e.manager.error_exit=jpeg_fail;
    if(setjmp(e.jump)) {
        jpeg_destroy_decompress(&j);
        std::fclose(in);
        throw std::runtime_error("JPEG decoding failed: "+source.string());
    }
    jpeg_create_decompress(&j);
    jpeg_stdio_src(&j,in);
    jpeg_save_markers(&j,JPEG_APP0+1,0xffff);
    jpeg_read_header(&j,TRUE);
    exif=orientation(j);
    j.out_color_space=JCS_RGB;
    jpeg_start_decompress(&j);
    if(!j.output_width||!j.output_height||j.output_width>16384||
       j.output_height>16384||j.output_components!=3)
        throw std::runtime_error("Invalid JPEG format");
    image=Image(int(j.output_width),int(j.output_height));
    while(j.output_scanline<j.output_height) {
        JSAMPROW row=image.at(0,int(j.output_scanline));
        jpeg_read_scanlines(&j,&row,1);
    }
    jpeg_finish_decompress(&j);
    jpeg_destroy_decompress(&j);
    std::fclose(in);
    return image;
}
static Image auto_orient(const Image& source,int orient) {
    if(orient==1)return source;
    const bool swapped=orient>=5;
    Image target(swapped?source.height:source.width,swapped?source.width:source.height);
    for(int y=0;y<target.height;y++)for(int x=0;x<target.width;x++) {
        int sx=x,sy=y;
        switch(orient) {
        case 2:sx=source.width-1-x;break;
        case 3:sx=source.width-1-x;sy=source.height-1-y;break;
        case 4:sy=source.height-1-y;break;
        case 5:sx=y;sy=x;break;
        case 6:sx=y;sy=source.height-1-x;break;
        case 7:sx=source.width-1-y;sy=source.height-1-x;break;
        case 8:sx=source.width-1-y;sy=x;break;
        default:break;
        }
        std::copy_n(source.at(sx,sy),3,target.at(x,y));
    }
    return target;
}
static std::uint8_t byte(double x) {
    return std::uint8_t(std::clamp(std::floor(x+.5),0.0,255.0));
}
static double grey(const std::uint8_t* p) {
    return .299*p[0]+.587*p[1]+.114*p[2];
}
// Match the original pipeline's light contrast, saturation, brightness order.
static void enhance(Image& im,double contrast,double colour,double brightness) {
    double mean=0;
    for(std::size_t i=0;i<im.pixels.size();i+=3)mean+=byte(grey(im.pixels.data()+i));
    mean=std::floor(mean/(im.pixels.size()/3)+.5);
    for(std::size_t i=0;i<im.pixels.size();i+=3) {
        auto* p=im.pixels.data()+i;
        for(int c=0;c<3;c++)p[c]=byte(mean+(p[c]-mean)*contrast);
        const double monochrome=byte(grey(p));
        for(int c=0;c<3;c++)p[c]=byte(monochrome+(p[c]-monochrome)*colour);
        for(int c=0;c<3;c++)p[c]=byte(p[c]*brightness);
    }
}
static double lanczos(double x) {
    x=std::abs(x);
    if(x>=3)return 0;
    if(x<1e-10)return 1;
    constexpr double pi=3.14159265358979323846;
    return 3*std::sin(pi*x)*std::sin(pi*x/3)/(pi*pi*x*x);
}
// Centered ImageOps-style cover cropping with antialiased Lanczos3 resampling.
static Image fit(const Image& src,int width,int height) {
    double cw=src.width,ch=src.height;
    const double ratio=double(width)/height;
    if(double(src.width)/src.height>ratio)cw=ch*ratio;
    else ch=cw/ratio;
    const double left=(src.width-cw)/2,top=(src.height-ch)/2;
    const double scale_x=cw/width,scale_y=ch/height;
    const double spread_x=std::max(1.,scale_x),spread_y=std::max(1.,scale_y);
    std::vector<float> horizontal(std::size_t(width)*src.height*3);
    for(int x=0;x<width;x++) {
        const double center=left+(x+.5)*scale_x-.5;
        const int lo=std::max(0,int(std::ceil(center-3*spread_x)));
        const int hi=std::min(src.width-1,int(std::floor(center+3*spread_x)));
        std::vector<std::pair<int,double>> weights;
        double sum=0;
        for(int s=lo;s<=hi;s++) {
            const double weight=lanczos((s-center)/spread_x);
            weights.emplace_back(s,weight);sum+=weight;
        }
        if(std::abs(sum)<1e-12)throw std::runtime_error("Empty horizontal resampling filter");
        for(int y=0;y<src.height;y++)for(int c=0;c<3;c++) {
            double value=0;
            for(auto [s,w]:weights)value+=src.at(s,y)[c]*w;
            horizontal[(std::size_t(y)*width+x)*3+c]=float(value/sum);
        }
    }
    Image dst(width,height);
    for(int y=0;y<height;y++) {
        const double center=top+(y+.5)*scale_y-.5;
        const int lo=std::max(0,int(std::ceil(center-3*spread_y)));
        const int hi=std::min(src.height-1,int(std::floor(center+3*spread_y)));
        std::vector<std::pair<int,double>> weights;
        double sum=0;
        for(int s=lo;s<=hi;s++) {
            const double weight=lanczos((s-center)/spread_y);
            weights.emplace_back(s,weight);sum+=weight;
        }
        if(std::abs(sum)<1e-12)throw std::runtime_error("Empty vertical resampling filter");
        for(int x=0;x<width;x++)for(int c=0;c<3;c++) {
            double value=0;
            for(auto [s,w]:weights)value+=horizontal[(std::size_t(s)*width+x)*3+c]*w;
            dst.at(x,y)[c]=byte(value/sum);
        }
    }
    return dst;
}
static void sharpen(Image& im) {
    constexpr double sigma=.65,amount=.60;
    constexpr int radius=2,threshold=4;
    double kernel[2*radius+1]{},total=0;
    for(int d=-radius;d<=radius;d++) {
        kernel[d+radius]=std::exp(-double(d*d)/(2*sigma*sigma));
        total+=kernel[d+radius];
    }
    for(double& v:kernel)v/=total;
    std::vector<float> horizontal(im.pixels.size()),blur(im.pixels.size());
    for(int y=0;y<im.height;y++)for(int x=0;x<im.width;x++)for(int c=0;c<3;c++) {
        double value=0;
        for(int d=-radius;d<=radius;d++)
            value+=kernel[d+radius]*im.at(std::clamp(x+d,0,im.width-1),y)[c];
        horizontal[(std::size_t(y)*im.width+x)*3+c]=float(value);
    }
    for(int y=0;y<im.height;y++)for(int x=0;x<im.width;x++)for(int c=0;c<3;c++) {
        double value=0;
        for(int d=-radius;d<=radius;d++)
            value+=kernel[d+radius]*horizontal[(std::size_t(std::clamp(y+d,0,im.height-1))*im.width+x)*3+c];
        blur[(std::size_t(y)*im.width+x)*3+c]=float(value);
    }
    for(std::size_t i=0;i<im.pixels.size();i++) {
        const double difference=double(im.pixels[i])-blur[i];
        if(std::abs(difference)>threshold)
            im.pixels[i]=byte(double(im.pixels[i])+amount*difference);
    }
}
static void encode_webp(const Image& im,const fs::path& target) {
    WebPConfig config{};
    if(!WebPConfigInit(&config)||!WebPConfigPreset(&config,WEBP_PRESET_PHOTO,86))
        throw std::runtime_error("Could not initialize WebP encoder");
    config.method=6;
    if(!WebPValidateConfig(&config))throw std::runtime_error("Unsupported WebP configuration");
    WebPPicture picture{};
    if(!WebPPictureInit(&picture))throw std::runtime_error("Could not allocate WebP picture");
    picture.width=im.width;picture.height=im.height;
    WebPMemoryWriter writer{};
    WebPMemoryWriterInit(&writer);
    picture.writer=WebPMemoryWrite;picture.custom_ptr=&writer;
    const bool good=WebPPictureImportRGB(&picture,im.pixels.data(),im.width*3)&&
                    WebPEncode(&config,&picture);
    const int error=picture.error_code;
    WebPPictureFree(&picture);
    if(!good) {
        WebPMemoryWriterClear(&writer);
        throw std::runtime_error("WebP encode failed: "+std::to_string(error));
    }
    int width=0,height=0;
    const bool valid=WebPGetInfo(writer.mem,writer.size,&width,&height)&&
                     width==im.width&&height==im.height;
    if(!valid) {
        WebPMemoryWriterClear(&writer);
        throw std::runtime_error("Invalid WebP output dimensions");
    }
    std::string template_name=target.string()+".XXXXXX";
    std::vector<char> temporary(template_name.begin(),template_name.end());
    temporary.push_back('\0');
    const int fd=mkstemp(temporary.data());
    if(fd<0) {
        WebPMemoryWriterClear(&writer);
        throw std::runtime_error("Cannot stage WebP output");
    }
    std::size_t written=0;
    bool ok=true;
    while(written<writer.size) {
        const ssize_t n=write(fd,writer.mem+written,writer.size-written);
        if(n<=0){ok=false;break;}
        written+=std::size_t(n);
    }
    if(close(fd)!=0)ok=false;
    if(ok&&std::rename(temporary.data(),target.c_str())!=0)ok=false;
    if(!ok)std::remove(temporary.data());
    WebPMemoryWriterClear(&writer);
    if(!ok)throw std::runtime_error("Could not publish WebP: "+target.string());
}
static void prepare(const fs::path& dir,const char* source,const char* output,
                    int width,int height,double contrast,double saturation,double brightness) {
    int exif=1;
    auto image=auto_orient(decode_jpeg(dir/source,exif),exif);
    enhance(image,contrast,saturation,brightness);
    auto resized=fit(image,width,height);
    sharpen(resized);
    encode_webp(resized,dir/output);
    std::cout<<"Prepared "<<output<<": "<<width<<"x"<<height<<'\n';
}
int main(int argc,char** argv) {
    try {
        if(argc!=2){std::cerr<<"Usage: prepare-garage-photos PHOTO_DIRECTORY\n";return 2;}
        const fs::path dir=argv[1];
        if(!fs::is_directory(dir))throw std::runtime_error("Missing photographs directory");
        prepare(dir,"500sec-amg-side.jpg","500sec-amg-three-quarter.webp",
                1021,409,1.045,1.035,1.02);
        prepare(dir,"500sec-amg-front.jpg","500sec-amg-front.webp",
                520,400,1.035,1.015,1.005);
        return 0;
    }catch(const std::exception& e) {
        std::cerr<<"Gallery preparation: "<<e.what()<<'\n';return 1;
    }
}
