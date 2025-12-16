# Flight Planning

    ## Dispatch-style trip brief (VFR/IFR) in plain English
    **Use when:** You want a concise, conservative go/no-go brief.  
    **Tags:** brief, route, fuel, alternates

    ```text
    You are my flight dispatcher and senior CFI.

Flight:
- Departure: [ICAO]
- Destination: [ICAO]
- Date/time (local): [TIME]
- Aircraft: [TYPE]
- Cruise: [KTAS] / [ALT]
- Fuel on board: [FOB] and burn [GPH]
- Pilot: [experience/currency]
- Passengers: [count + experience]
- Constraints: [night? terrain? avoid Bravo?]

Provide:
1) Route recommendation(s) with airspace/terrain gotchas
2) Fuel plan with reserves (state assumptions)
3) Alternate strategy (VFR/IFR as applicable)
4) Top 5 risks + mitigations
5) Go / delay / no-go recommendation with rationale

Ask only the minimum follow-up questions needed.
    ```

    ## Fuel planning sanity check
    **Use when:** You want to verify legs, reserves, and alternates.  
    **Tags:** fuel, reserve, range

    ```text
    Act as a conservative GA operator.

Inputs:
- Aircraft: [TYPE], cruise burn [GPH], taxi burn [GPH or assume], usable fuel [GAL]
- Route: [DEP]-[ENROUTE STOPS]-[DEST]
- Distances/ETEs: [PASTE]
- Winds aloft: [PASTE or 'unknown']
- Required reserve: [VFR 30/45 or IFR 45 + alternate etc.]

Compute a fuel plan per leg:
- Planned fuel used
- Fuel remaining on landing
- Reserve margin

Flag any leg that is tight and propose fixes (altitude, power, stop, alternate). State assumptions.
    ```

    ## Airspace and Bravo/B shelves check
    **Use when:** You want a reminder of common airspace traps.  
    **Tags:** airspace, bravo, route

    ```text
    You are a CFI. I will paste my route description and intended altitudes.

Task:
- Identify likely airspace conflicts (Class B/C/D, MOAs, restricted, SFRA, TFR risk areas)
- Suggest 2 route/altitude options that reduce workload
- Provide a short checklist of what to verify on the chart and in NOTAMs

Route/altitudes:
[PASTE HERE]

Do not invent frequencies or clearances. If you need more info, ask.
    ```

    ## Performance and density altitude quick check
    **Use when:** You’re operating on a warm day or high field elevation.  
    **Tags:** performance, DA, takeoff

    ```text
    Act as a cautious performance engineer + CFI.

Given:
- Aircraft: [TYPE]
- Airport: [IDENT], elevation [FT]
- Temp/dewpoint: [C/F]
- Altimeter setting: [INHG]
- Runway length/surface/slope: [DETAILS]
- Weight: [LB], CG: [if known]
- Obstacles/terrain: [DETAILS]

Estimate density altitude and discuss performance risk qualitatively.
List what to compute/verify in POH and what “stop signs” would make you reduce weight, wait, or choose another runway/time.
    ```

    ## Cross-country nav log helper
    **Use when:** You want a quick, structured nav log outline.  
    **Tags:** navlog, planning, wx

    ```text
    You are my CFI. Create a nav log outline for:
- Route: [DEP] to [DEST] via [FIXES]
- Cruise altitude: [ALT]
- Cruise speed: [KTAS]
- Winds aloft: [PASTE]
- Fuel burn: [GPH]

Output:
- A nav log table structure (legs, headings, wind correction, groundspeed, ETE, fuel)
- A short checklist for preflight planning items (wx, NOTAMs, weight/balance, alternates)
Do not fabricate numbers I did not provide; mark unknowns clearly.
    ```

    ## Passenger briefing script (GA)
    **Use when:** You’re taking non-pilot passengers up.  
    **Tags:** passenger, brief, safety

    ```text
    Write a clear passenger briefing script for a GA flight.

Flight context:
- Aircraft: [TYPE]
- Duration: [TIME]
- Conditions: [smooth/choppy, day/night]
- Passengers: [who + experience level]

Include:
- Seat belts/shoulder harness, doors, headsets, sterile cockpit
- Air sickness prevention
- Emergency items and evacuation guidance
- What to do if I say “my aircraft”
Keep it calm and short.
    ```

