# Weather

    ## Weather quicklook + red flags (METAR/TAF/AIRMET/SIGMET)
    **Use when:** You have raw weather products and want an operational summary.  
    **Tags:** metar, taf, sigmet, icing

    ```text
    Act as an experienced GA weather briefer.

I will paste METARs/TAFs/AIRMETs/SIGMETs below.

1) Summarize the big picture for a light single / light twin
2) Identify red flags and why they matter
3) Give an operational recommendation (VFR / IFR / delay / cancel)
4) Provide “watch items” over the next 3–6 hours

Weather:
[PASTE HERE]
    ```

    ## Ceilings/visibility trend assessment
    **Use when:** You’re concerned about marginal VFR or improving/deteriorating trend.  
    **Tags:** mvfr, trend, ceiling

    ```text
    You are a conservative GA briefer.

Inputs:
- Departure: [ICAO] METAR/TAF: [PASTE]
- Destination: [ICAO] METAR/TAF: [PASTE]
- Alternates: [ICAO(s)] METAR/TAF: [PASTE]
- Planned arrival: [TIME local]

Assess:
- When ceilings/vis are most likely to be worst/best (based on TAF timing)
- Whether the trend supports a safe return option
- A decision rule: “If X at time Y, then delay/divert/cancel”
State assumptions.
    ```

    ## Convective risk decision helper
    **Use when:** Thunderstorms are in the picture and you want a conservative plan.  
    **Tags:** ts, convective, risk

    ```text
    Act as a conservative GA operator.

I will paste convective outlook, radar description, and TAFs.

Provide:
- Why convective weather is high risk for GA
- A conservative “go/no-go” framework
- Options: earlier/later departure, route changes, stay on ground
- What additional info to obtain before deciding

Data:
[PASTE HERE]
    ```

    ## Icing risk (GA) — talk me out of a bad idea
    **Use when:** Freezing levels / clouds / precip make you uneasy.  
    **Tags:** icing, freezing, risk

    ```text
    You are my safety officer and IFR instructor.

Inputs:
- Aircraft: [TYPE], ice protection: [none / TKS / boots]
- Route/altitudes: [PASTE]
- Temps/freezing levels: [PASTE]
- Cloud layers: [PASTE]
- Precip: [PASTE]
- PIREPs: [PASTE]

Assess icing exposure risk. Be conservative.
If risk is medium/high, propose safer alternatives (delay, reroute, altitude changes, different destination).
State assumptions and ask only necessary follow-ups.
    ```

    ## Winds/gusts and crosswind planning
    **Use when:** You’re dealing with gusty conditions and want a landing plan.  
    **Tags:** wind, crosswind, gusts

    ```text
    Act as a CFI coaching conservative crosswind operations.

Inputs:
- Airport/runway(s): [IDENT + RWYs]
- Reported winds: [e.g., 24012G22]
- Aircraft: [TYPE], personal max crosswind: [KT]
- Surface/conditions: [dry/wet/short]

Compute/estimate crosswind component per runway.
Recommend:
- Best runway choice
- Approach/landing technique reminders
- Decision points for go-around/divert
State assumptions.
    ```

