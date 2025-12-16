# IFR

    ## IFR flight briefing (systematic)
    **Use when:** You want a structured IFR risk and plan review.  
    **Tags:** ifr, alternates, minima

    ```text
    You are my IFR instructor.

Flight:
- DEP/DEST: [ICAO] → [ICAO]
- ETA: [TIME]
- Aircraft/equipment: [TYPE], [WAAS/non-WAAS], autopilot [yes/no]
- Pilot: [currency]
- Approaches available at DEST: [LIST if known]
- Weather products: [PASTE METAR/TAF/AIRMET]

Deliver:
- Best approach strategy and backup
- Alternate requirement discussion (state assumptions)
- Key IFR risks (icing, convective, winds, ceilings)
- Programming/automation plan (autopilot use, brief approach, missed)
Ask only the minimum follow-ups needed.
    ```

    ## Approach brief generator (ILS/RNAV/VOR)
    **Use when:** You want a crisp approach brief you can read aloud.  
    **Tags:** approach, brief, missed

    ```text
    Create an approach brief template and then fill it using this info:

- Approach name: [e.g., RNAV (GPS) RWY 04]
- Airport: [ICAO]
- Final approach fix: [FAF]
- DA/MDA: [value]
- Missed approach: [PASTE text]
- Notes: [winds, runway length, lighting, circling?]

Output should be:
- Setup items
- Final segment callouts
- Stabilized approach gates
- Missed approach actions
Do not invent chart details I did not provide.
    ```

    ## Holding clearance practice
    **Use when:** You want to get fast at understanding holding instructions.  
    **Tags:** hold, clearance, practice

    ```text
    Act as ATC and give me 10 holding clearances of varying complexity.

For each:
1) Give the clearance
2) Wait for my readback and hold entry plan
3) Grade my plan and correct it if needed

Keep it general (no real frequencies).
    ```

    ## Lost comms thought exercise (IFR)
    **Use when:** You want to rehearse decision making if radios fail.  
    **Tags:** lost comms, ifr, decision

    ```text
    Act as an IFR instructor.

Scenario:
- Location/phase of flight: [enroute/arrival/approach]
- Clearance: [PASTE]
- Weather: [PASTE]
- Equipment failure: [COM1/COM2/Audio panel/etc.]
- Terrain/airspace: [NOTES]

Walk through:
- Immediate actions and troubleshooting
- How to decide what to fly next (state applicable logic, but do not quote regs verbatim)
- How to minimize risk and workload
Ask only essential follow-ups.
    ```

