# Comms

    ## Towered radio call scripts (departure to landing)
    **Use when:** You want concise phraseology sequences to rehearse.  
    **Tags:** towered, phraseology, script

    ```text
    You are a CFI drilling comms.

Scenario:
- Airport: [IDENT]
- Runway: [RWY]
- Aircraft: [TYPE] / callsign: [CALLSIGN]
- Operation: [VFR departure / VFR arrival / pattern work / practice approach]
- Notes: [taxi instructions complexity, hotspots, etc.]

Write concise call scripts in order (who I call, what I say).
Do NOT invent frequencies.
    ```

    ## Non-towered pattern comms script
    **Use when:** You want a clean, predictable CTAF script.  
    **Tags:** ctaf, pattern, calls

    ```text
    You are a CFI. Write non-towered CTAF calls for:

- Airport: [IDENT]
- Runway: [RWY]
- Entry: [45/downwind/base/straight-in]
- Aircraft/callsign: [CALLSIGN]
- Pattern altitude: [ALT]

Include: taxi/departure, inbound 10 miles, entry, downwind/base/final, clear of runway.
Keep it short and standard. Do NOT invent frequencies.
    ```

    ## IFR readback practice
    **Use when:** You want to rehearse IFR clearance readbacks.  
    **Tags:** ifr, clearance, readback

    ```text
    Act as ATC and give me 5 realistic IFR clearances for:
- Departure airport: [ICAO]
- Destination: [ICAO]
- Aircraft: [TYPE], equipment: [RNAV capable?]
- Route preference: [direct / via fixes]

Then act as my CFI and grade my readback:
- What I said right
- What I missed
- A corrected readback format

Wait for my readback before grading.
    ```

    ## Busy frequency compression drill
    **Use when:** You want to practice saying only what matters.  
    **Tags:** workload, brevity, comms

    ```text
    You are a strict CFI.

Give me 10 scenarios where I must make a single concise transmission:
- Who I'm calling
- My position/altitude
- My request

After each, critique for brevity and correctness. No made-up frequencies.
    ```

