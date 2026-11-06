#!/usr/bin/env python3
"""Prepare non-destructive gallery copies of original 500SEC photographs.

Original JPEGs remain the authoritative archival sources. No vehicle details,
number plates, backgrounds or bodywork are synthesised or replaced.
"""
from pathlib import Path

from PIL import Image, ImageEnhance, ImageFilter, ImageOps

PHOTOS = Path(__file__).resolve().parents[1] / "assets" / "photos"


def prepare(source: str, destination: str, size: tuple[int, int], *,
            contrast: float, colour: float, brightness: float) -> None:
    with Image.open(PHOTOS / source) as original:
        image = ImageOps.exif_transpose(original).convert("RGB")

    image = ImageEnhance.Contrast(image).enhance(contrast)
    image = ImageEnhance.Color(image).enhance(colour)
    image = ImageEnhance.Brightness(image).enhance(brightness)
    image = ImageOps.fit(image, size, method=Image.Resampling.LANCZOS)
    image = image.filter(
        ImageFilter.UnsharpMask(radius=0.65, percent=60, threshold=4)
    )
    target = PHOTOS / destination
    image.save(target, format="WEBP", quality=86, method=6)
    with Image.open(target) as check:
        if check.format != "WEBP" or check.size != size:
            raise RuntimeError(f"Invalid gallery photograph: {target}")
    print(f"Prepared {destination}: {size[0]}x{size[1]}")


if __name__ == "__main__":
    prepare("500sec-amg-side.jpg", "500sec-amg-three-quarter.webp",
            (1021, 409), contrast=1.045, colour=1.035, brightness=1.02)
    prepare("500sec-amg-front.jpg", "500sec-amg-front.webp",
            (520, 400), contrast=1.035, colour=1.015, brightness=1.005)
