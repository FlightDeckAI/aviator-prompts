# Emergencies & Abnormals

    ## Engine failure after takeoff (EFATO) chair brief
    **Use when:** You want a crisp, phase-of-flight brief before departure.  
    **Tags:** efato, brief, emergency

    ```text
    You are my CFI. Create an EFATO brief for:

- Airport/runway: [IDENT/RWY]
- Departure path/terrain: [NOTES]
- Aircraft: [TYPE]
- Weather/wind: [PASTE]
- Pattern altitude: [ALT]

Output:
- A short “before takeoff” verbal brief I can read aloud
- Decision gates (altitudes/speeds) expressed generically
- What to avoid (common killer moves)
Do not invent airport-specific obstacles I did not provide.
    ```

    ## Alternator failure: checklist + decision path
    **Use when:** You suspect charging system problems.  
    **Tags:** electrical, alternator, abnormal

    ```text
    Act as a GA instructor. For an alternator/charging failure scenario:

Inputs:
- Aircraft: [TYPE]
- Symptoms: [low volts, ammeter, light, etc.]
- Phase of flight: [enroute/IMC/night]

Provide:
- Immediate actions (high level)
- Workload management priorities
- Land ASAP vs continue decision factors
- A short passenger briefing (if applicable)

Avoid aircraft-specific POH steps unless I paste them.
    ```

    ## Smoke in cockpit: decision checklist
    **Use when:** You need a conservative response framework.  
    **Tags:** smoke, fire, emergency

    ```text
    You are my safety officer.

Scenario:
- Aircraft: [TYPE]
- Phase of flight: [taxi/takeoff/enroute/approach]
- Suspected source: [electrical/cabin/unknown]
- Environment: [IMC/VMC, day/night, terrain]

Create:
- Immediate actions (high level)
- Priorities: aviate/navigate/communicate
- Divert and landing strategy
- Post-landing actions
Be conservative and keep it readable.
    ```

    ## Unstable approach and go-around discipline
    **Use when:** You want a hard rule and script to avoid pressing.  
    **Tags:** go-around, stabilized, discipline

    ```text
    Act as a strict CFI.

Aircraft: [TYPE]
Runway: [IDENT/RWY], length: [FT]
Conditions: [gusty/calm], traffic: [light/busy]

Define:
- Stabilized approach criteria (simple)
- A go-around trigger list (bullet)
- A short go-around verbal script (callouts + actions)
Keep it generic and conservative.
    ```

