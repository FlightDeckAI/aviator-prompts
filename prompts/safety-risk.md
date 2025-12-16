# Safety & Risk

    ## PAVE + 5P risk assessment (rapid)
    **Use when:** You want a quick risk picture and mitigations.  
    **Tags:** pave, 5p, risk

    ```text
    Run a rapid PAVE + 5P assessment for this flight.

Inputs:
- Pilot: [experience, currency, fatigue]
- Aircraft: [type, status, deferred items]
- enVironment: [wx, terrain, night, icing]
- External pressures: [schedule, passengers]
- Plan/Plane/Pilot/Passengers/Programming: [notes]

Output:
- Risk score (low/med/high) with reasons
- Top 5 mitigations
- Go/no-go recommendation
Ask only the minimum follow-ups to decide.
    ```

    ## Personal minimums builder
    **Use when:** You want to define minimums that match your experience.  
    **Tags:** minimums, go-no-go, personal

    ```text
    Act as a CFI. Help me define personal minimums.

My profile:
- Rating(s): [PPL/IR/etc.]
- Hours: [total], last 90 days: [hours]
- Aircraft: [TYPE]
- Typical mission: [local/x-country/night/passengers]

Create a one-page personal minimums table for:
- Winds/crosswind/gusts
- Ceilings/visibility (VFR and IFR if applicable)
- Night ops
- Icing/convective avoidance rules
- Passenger/pressure limits
Include a 3-step “raise minimums temporarily when…” rule.
    ```

    ## Go/no-go decision memo
    **Use when:** You want a clean written rationale for your choice.  
    **Tags:** decision, brief, risk

    ```text
    Write a concise go/no-go memo for this flight.

Inputs:
- Mission: [why I’m flying]
- Alternatives: [drive/airline/delay]
- Constraints: [time, passengers]
- Weather summary: [PASTE]
- Aircraft status: [NOTES]
- Pilot status: [fatigue/currency]

Output:
- Decision (go/delay/no-go)
- Rationale (bullet points)
- Mitigations (if go)
- Re-evaluation triggers (what would change my mind)
    ```

    ## Threat and error management (TEM) plan
    **Use when:** You want a proactive TEM plan for a specific flight.  
    **Tags:** tem, threats, mitigation

    ```text
    Act as a safety pilot and CFI.

Flight summary:
[PASTE]

Create a TEM table:
- Threat
- Possible error
- Undesired aircraft state
- Countermeasure (specific, actionable)

End with 5 “callouts” I should say aloud during the flight.
    ```

