# ssmithnet.net — Design Language

## Purpose

`ssmithnet.net` is Shannon Smith's personal corner of the internet. It should feel like a carefully engineered, long-lived object rather than a fashionable portfolio, dashboard, or generic technology landing page.

The design reference is not "minimalism". It is the character of a well-made Mercedes-Benz or Rolls-Royce: substantial, materially rich, technically deep, refined, and confident enough not to display every piece of engineering on the surface.

The governing principle is:

> **Quietly substantial. Technically deep. Materially rich. Deliberately restrained.**

A useful shorthand is **"the best or nothing"**: use the best solution available, whether it is new, old, mechanical, digital, simple, or complex. Modernity is not a virtue by itself. Refinement is.

## How it should feel

The visitor should initially perceive calm, quality, confidence and personality. As they explore, they should discover more depth, detail and capability.

The interface should feel like a beautifully engineered interior: the structure is doing a great deal of work, but the structure itself should not constantly call attention to itself.

The site may contain a lot of information. It should not look like it is trying to prove how much information it contains.

Subtlety is therefore not the absence of richness. It is **richness that does not shout**.

## Visual character

Use a dark blue-black foundation with controlled cool blue/cyan accents and restrained warm material notes. Black, silver, graphite, cool blue and small amounts of warm wood/leather influence are appropriate. Avoid bright multicolour palettes and avoid literal automotive theming.

The existing MB Corpo/MB Head typography is part of the identity and should remain central. Typography should feel precise, mature and slightly technical without becoming sterile.

Natural materials are a useful conceptual reference: leather, brushed metal, silver, dark wood and glass. These should influence colour, texture, depth and lighting rather than becoming literal decorative textures everywhere.

Original linework, faint technical meshes and other background graphics are welcome when they provide atmosphere. They must remain subordinate to the content and fade appropriately on smaller screens.

## What to avoid

Do not turn the site into a generic SaaS dashboard.

Do not make every section a repeated sequence of kicker + heading + paragraph + rounded card + button.

Do not use borders, separators or boxes simply because they are available in CSS. Containers should communicate real grouping.

Do not confuse "Mercedes" with "sparse". Mercedes can be richly equipped and materially luxurious. The lesson is disciplined integration and hidden engineering.

Do not flatten everything into the same visual level. A current project, a historical car, a technical note and a navigation link do not all need the same prominence.

Do not overuse uppercase cyan labels. They are a navigation/detail language, not a decoration to place above every object.

Do not make the front page a summary of every other page. The home page is the front hall: it should orient and tempt exploration, while the deeper pages own their subjects.

Do not make the copy explain the design. Phrases such as "the other cars get the same treatment", "this page only needs to point", or similar commentary about how the site is arranged should normally be replaced with language that describes the subject itself. The design should demonstrate the principle rather than announce it.

Do not use "other" as a synonym for "less important". When grouping historic material, distinctions should describe chronology, geography or subject matter rather than imply rank.

## Information architecture

### Home

The front page should establish the personality of the site, give a concise sense of what Shannon is interested in now, and provide clear routes into the deeper areas.

It should **tease rather than duplicate**.

Detailed technical project information belongs in Workbench. Detailed car information belongs in Garage. The 1990s history and beta-testing material belongs in Archive.

The Heinlein quotation can remain as a personal closing statement on the home page.

### Workbench

Workbench is the technical workshop. It should contain the serious project detail: InfiltratorFS, the LINK family and personal software/systems work that Shannon actually cares about.

LINK must be represented as an **automotive family**, not as a synonym for MBLINK. MBLINK is the Mercedes member; other make-specific projects belong underneath the shared LINK architecture.

Employment tooling that Shannon does not consider a personal interest should not be presented as part of his identity merely because he works with it.

The subtle Amiga reference in the name **Workbench** is intentional and can be reinforced through wording and small discoveries without turning the site into an Amiga fan page.

### Garage

Garage is the authoritative home for vehicle history.

The current car is the **2011 Mercedes-Benz E 250 CDI BlueEFFICIENCY Coupé (C207)**. Its hero treatment may be richer and more detailed because verified VIN/catalogue data exists for it. This is deliberate and should not be "normalised away" merely for symmetry.

Historical vehicles should use a standardised data-card model so that different makes are treated with equal respect. Recommended common fields are:

- Make
- Model
- Chassis / Series
- Powertrain

Add another field only where it represents a genuinely meaningful difference. A hero vehicle may legitimately expose additional verified information.

The historical Mercedes chapter includes the W116 450SEL 6.9, W116 280SE, and C126 500SEC that was AMG.

The Australian/other chapter is equally important. It includes the EL Falcon with Tickford-installed LPG, XF Falcon, 2003 BA Fairmont 5.4 V8 with LPG dual fuel, LC Torana with 1.1-litre four-cylinder engine, 2004 BA LTD 5.4 V8 with LPG dual fuel, AU Series II Fairmont 4.0-litre with LPG dual fuel, Land Rover Discovery 2 TDI and Peugeot 504 Diesel that Shannon ran on waste motor oil.

The Australian cars are not an "other" or lesser class. The distinction is historical and cultural, not a quality ranking.

### Archive

Archive owns the history of the site: the 1990s origin, the Mercedes pages that came first, beta-testing history and other older material that rewards exploration.

Do not duplicate the full history elsewhere when it belongs here.

## Content tone

Write as though the site already has confidence. Avoid marketing language, exaggerated claims and generic technology copy.

Good copy is concise, specific and slightly personal. The site can have dry humour and small surprises. It should never sound like a corporate brochure.

Do not write about the site's own layout, hierarchy or design choices unless the subject genuinely calls for it. The visitor should experience the design rather than be told how it works.

The user's interests should feel connected by a common philosophy rather than catalogued as a résumé.

## Interaction philosophy

Important information should be easy to discover, but not every discoverable thing needs to be announced as a button.

Prefer clickable headings, quiet text links, progressive disclosure, tabs, compact registers, specification blocks and well-placed navigation over a sea of call-to-action buttons.

The best interactions should feel obvious after discovery, not loudly advertised beforehand.

## Responsive behaviour

Desktop and mobile are the same design language, not separate compositions.

On desktop, use hierarchy, columns and negative space to create calm. On mobile, preserve the same hierarchy rather than simply stacking every desktop block and retaining all desktop ornament.

Background graphics should simplify and fade on small screens. Navigation must remain immediately usable. Typography must remain intentional rather than relying on arbitrary scaling.

Every page must be tested as both a desktop and mobile experience before considering the UI finished.

## Standardisation rule

When adding new content, first determine what type of object it is and place it into the existing visual language. Do not invent a new card style, badge style, heading treatment or spacing convention for each new item.

Variation should come from hierarchy, content and scale — not from accumulating components.

## Final test

Before accepting a design change, ask:

1. Does this look like a carefully engineered object rather than a template?
2. Is the richness present without becoming visual noise?
3. Is the underlying complexity discoverable without being forced onto the visitor?
4. Does the content live on the page where it belongs rather than being repeated everywhere?
5. Does an older, cheaper, stranger or less fashionable object receive the same respect as an expensive or prestigious one?
6. Does the copy describe the subject rather than explain the design?
7. Does the page still feel unmistakably like Shannon's site?

If the answer to these questions is yes, the design is moving in the right direction.