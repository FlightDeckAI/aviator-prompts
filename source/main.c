#include <nds.h>
#include <fat.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <errno.h>

#define NUM_DAYS 56
#define NOTE_MAX 256
#define NUM_REQ_CHECK 7
#define NUM_REQ_HOURS 8
#define NUM_FLASHCARDS 24
#define TAB_COUNT 4
#define LINE_MAX 64
#define VIEW_LINES 16

typedef struct {
    const char *label;
    const char *flight;
    const char *ground;
    const char *homework;
} DayData;

typedef struct __attribute__((packed)) {
    char magic[8];
    u16 version;
    u16 currentDay;
    u8 dayFlags[NUM_DAYS];
    char notes[NUM_DAYS][NOTE_MAX];
    u8 reqCheckbox[NUM_REQ_CHECK];
    s32 reqHours[NUM_REQ_HOURS];
    u16 flashcardIndex;
    u8 flashcardReveal;
} SaveData;

static const DayData gDays[NUM_DAYS] = {
    {"Week 1 Day 1",
     "- Intro flight\n- Preflight\n- Taxi\n- Straight-and-level\n- Climbs/descents\n- Medium turns",
     "- Training flow\n- Checklists\n- Basics of aerodynamics + control effects",
     "- Chair-fly: start->taxi->runup->takeoff->pattern->landing\n- Begin daily weather brief habit"},
    {"Week 1 Day 2",
     "- Fundamentals\n- Trim practice\n- Slow flight intro\n- Basic pattern orientation",
     "- Airspace intro\n- Airport ops\n- Basic radio phraseology",
     "- Practice radio calls\n- Memorize POH limits + V-speeds"},
    {"Week 1 Day 3",
     "- Slow flight\n- Power-off stall intro\n- Power-on stall intro",
     "- Stall/spin awareness\n- Angle of attack\n- Pattern safety",
     "- Chair-fly stall recognition + recovery steps"},
    {"Week 1 Day 4",
     "- Pattern work\n- Normal takeoff\n- Normal landing\n- Go-around",
     "- Runway incursion avoidance\n- Pattern entries\n- Wind correction basics",
     "- Landing errors & corrections notes"},
    {"Week 1 Day 5",
     "- Landings (reps)\n- Crosswind technique intro\n- Slips overview",
     "- Weather basics: METAR/TAF\n- Density altitude concept",
     "- Do a real weather brief daily (METAR/TAF/winds)"},
    {"Week 1 Day 6",
     "- Ground reference maneuvers intro\n- A few landings",
     "- Sectional chart basics\n- Local landmarks planning",
     "- Identify 10 landmarks + 5 emergency fields"},
    {"Week 1 Day 7 (Catch-up / Rest)",
     "",
     "- Review weak areas\n- Chair-fly full pattern flows",
     "- Light study + rest\n- Prep for Week 2 maneuvers"},
    {"Week 2 Day 8",
     "- Steep turns intro\n- Slow flight refine\n- 2-3 landings",
     "- Load factor\n- Stalls vs spins\n- Risk management: IMSAFE/PAVE",
     "- Steep turn sight picture notes"},
    {"Week 2 Day 9",
     "- Short-field takeoff/landing technique intro",
     "- Performance planning concepts\n- Stabilized approach idea",
     "- Compute POH takeoff/landing for 3 scenarios"},
    {"Week 2 Day 10",
     "- Soft-field takeoff/landing technique intro",
     "- Soft-field aerodynamics + technique",
     "- Chair-fly soft-field flows"},
    {"Week 2 Day 11",
     "- Emergency procedures intro\n- Engine failure best glide\n- Forced landing setup",
     "- Emergency checklists\n- Glide planning decision-making",
     "- Review POH emergency items (CFI-approved)"},
    {"Week 2 Day 12",
     "- Navigation exercise #1 (local)\n- Diversion intro",
     "- Navlog basics\n- Checkpoints\n- Pilotage/dead reckoning",
     "- Build a short navlog for a local route"},
    {"Week 2 Day 13 (Stage Check 1)",
     "- Mini-checkride profile\n- Fundamentals\n- Stalls\n- Landings\n- Go-arounds",
     "- Oral-style quiz over covered topics",
     "- Write top 5 weak points + fixes"},
    {"Week 2 Day 14 (Catch-up / Rest)",
     "",
     "- Knowledge test prep ramp-up\n- Review missed concepts",
     "- Practice test set + review errors"},
    {"Week 3 Day 15",
     "- Pattern mastery day\n- Crosswind corrections\n- Go-arounds",
     "- Solo requirements + limitations\n- Airport-specific procedures",
     "- Chair-fly 10 perfect patterns"},
    {"Week 3 Day 16",
     "- Pre-solo dual standards day\n- Consistency focus",
     "- Pre-solo written/oral items (CFI)",
     "- Pattern callouts + flows review"},
    {"Week 3 Day 17 (First Solo Attempt)",
     "- If endorsed: first solo (pattern)\n- If not: dual polish to solo standard",
     "- Solo risk briefing\n- Personal minimums",
     "- Debrief journal: what surprised you?"},
    {"Week 3 Day 18",
     "- Dual tune-up\n- Solo consolidation (if approved)",
     "- ADM: stop criteria\n- Fatigue management",
     "- Chair-fly go-around triggers"},
    {"Week 3 Day 19",
     "- Local nav\n- Simulated lost procedures",
     "- Lost procedures\n- Diversion math\n- Flight following basics",
     "- Plan dual XC candidate route for next week"},
    {"Week 3 Day 20 (Knowledge Test Target)",
     "",
     "- Take FAA knowledge test (or final practice exams)",
     "- Review missed knowledge areas immediately"},
    {"Week 3 Day 21 (Catch-up / Rest)",
     "",
     "- Correct knowledge gaps\n- Chair-fly + rest",
     "- Light review; prepare XC phase"},
    {"Week 4 Day 22 (Dual XC #1)",
     "- Dual cross-country\n- 2-3 airports\n- Pilotage/DR\n- Flight plan usage",
     "- Full XC planning brief\n- Postflight review",
     "- Navlog cleanup\n- Identify 3 what-if changes"},
    {"Week 4 Day 23 (Dual XC #2)",
     "- Dual XC with diversion\n- Lost procedures scenario",
     "- Airspace along route\n- Weather decision points",
     "- Create solo XC candidate route list"},
    {"Week 4 Day 24",
     "- Instrument training under hood\n- Unusual attitudes recovery",
     "- Scan basics\n- Inadvertent IMC avoidance concepts",
     "- Chair-fly instrument scan"},
    {"Week 4 Day 25 (Night #1)",
     "- Night pattern work\n- Start night landing count",
     "- Night illusions\n- Night risk management",
     "- Night landing notes + illusion risks list"},
    {"Week 4 Day 26 (Night XC)",
     "- Night XC (>100 NM total)\n- Conservative outs + alternates",
     "- Pre-brief route risks + alternates",
     "- Postflight: what was different at night?"},
    {"Week 4 Day 27",
     "- Solo consolidation (pattern/local) OR dual as needed\n- Towered plan prep",
     "- Towered ops review\n- Light gun signals\n- Comms expectations",
     "- Script towered calls"},
    {"Week 4 Day 28 (Catch-up / Rest)",
     "",
     "- Requirement tracking audit\n- Weak area review",
     "- Rest + chair-fly XC flows"},
    {"Week 5 Day 29 (Solo XC #1)",
     "- First solo XC (conservative route)",
     "- Thorough pre-brief: winds/alternates/minimums",
     "- Debrief: timing/fuel accuracy vs plan"},
    {"Week 5 Day 30",
     "- Dual polish\n- Short/soft field\n- Slips\n- Crosswind landings",
     "- Performance + limitations\n- Go/no-go decisions",
     "- Oral flashcards: weak areas"},
    {"Week 5 Day 31 (Solo XC #2)",
     "- Solo XC building time\n- Diversion readiness",
     "- Pre-brief diversion options",
     "- Write a diversion template (steps)"},
    {"Week 5 Day 32",
     "- Dual emergency day\n- Engine failure on takeoff (sim)\n- Emergency descent\n- System issues scenarios",
     "- Systems + malfunctions\n- Single-pilot resource management",
     "- Emergency checklist familiarity (POH)"},
    {"Week 5 Day 33 (Solo Long XC)",
     "- Solo XC 150 NM total\n- 3 full-stop landings\n- One leg >50 NM",
     "- Conservative weather mins\n- Hard abort criteria",
     "- Detailed debrief + lessons learned"},
    {"Week 5 Day 34",
     "- Mock checkride maneuvers\n- Steep turns\n- Stalls\n- Ground reference\n- Instrument\n- Emergency approach",
     "- Oral prep: explain WHY + risk management",
     "- 30-45 min oral rehearsal out loud"},
    {"Week 5 Day 35 (Catch-up / Rest)",
     "",
     "- Fix discovered weak points\n- Rest",
     "- Light study; prep next phase"},
    {"Week 6 Day 36",
     "- Instrument completion (if needed)\n- Pattern polish",
     "- Airspace deep dive",
     "- Airspace flashcards"},
    {"Week 6 Day 37 (Night Landings Finish)",
     "- Night full-stop landings to complete requirement (as needed)",
     "- Night ADM review\n- Mitigations",
     "- Night pitfalls + mitigations list"},
    {"Week 6 Day 38 (Towered Solo Req)",
     "- Solo: 3 takeoffs + 3 full-stop landings at towered airport",
     "- Towered comms expectations",
     "- Debrief towered ops errors + fixes"},
    {"Week 6 Day 39",
     "- Scenario XC: 'go to lunch' mission\n- Constraints + alternates",
     "- Flight following\n- Practical use of weather tools",
     "- Build a checkride XC plan packet"},
    {"Week 6 Day 40 (Stage Check 2)",
     "- Full mock ride flight profile",
     "- Timed mock oral",
     "- List top 10 oral questions to tighten"},
    {"Week 6 Day 41 (Ground Intensive)",
     "",
     "- Oral bootcamp (ACS-style)\n- Explain decisions + risks",
     "- Record answers; replay and refine"},
    {"Week 6 Day 42 (Catch-up / Rest)",
     "",
     "- Paperwork review\n- Checkride scheduling readiness\n- Endorsements check",
     "- Rest + light review"},
    {"Week 7 Day 43",
     "- Checkride prep flight #1\n- Landings + go-arounds to ACS tolerances",
     "- Oral: privileges/limitations\n- Required documents",
     "- Documents + endorsements checklist"},
    {"Week 7 Day 44",
     "- Checkride prep flight #2\n- Maneuvers + emergency integration",
     "- Oral: weather interpretation + go/no-go",
     "- Weather decision scenarios (5)"},
    {"Week 7 Day 45",
     "- Solo confidence flight OR dual targeted fixes",
     "- Oral: XC planning + airspace + ADM",
     "- Chair-fly entire ride start-to-finish"},
    {"Week 7 Day 46 (Mock Oral)",
     "",
     "- Full mock oral (ideally with second instructor)",
     "- Fix weakest 2 areas"},
    {"Week 7 Day 47",
     "- Checkride prep flight #3\n- Full mock practical profile",
     "- Debrief to a punch list (<=5 items)",
     "- Punch list study"},
    {"Week 7 Day 48 (Catch-up / Rest)",
     "",
     "- Logbook + requirement audit\n- IACRA/application check",
     "- Rest + calm review"},
    {"Week 8 Day 49",
     "- Final polish (short flight)\n- Don't overtrain",
     "- Calm oral review\n- No new material",
     "- Rest and confidence routine"},
    {"Week 8 Day 50",
     "",
     "- Checkride readiness review\n- Sleep + hydration plan",
     "- Light review only"},
    {"Week 8 Day 51 (Checkride Target)",
     "- Practical test (oral + flight)",
     "- Stay calm; use checklists; manage risk",
     "- Post-ride notes (what to improve next)"},
    {"Week 8 Day 52 (Buffer)",
     "- If weathered out: short proficiency flight\n- Keep momentum",
     "- Review weak ACS areas only",
     "- Rest"},
    {"Week 8 Day 53 (Buffer)",
     "",
     "- If discontinuance: focus only retest items",
     "- Light prep for retest"},
    {"Week 8 Day 54",
     "",
     "- Post-cert briefing\n- Passenger brief\n- Personal minimums\n- Next 10 hours plan",
     "- Set near-term proficiency goals"},
    {"Week 8 Day 55 (Rest)",
     "",
     "- Rest day",
     "- Optional: gentle review"},
    {"Week 8 Day 56 (Closeout)",
     "",
     "- Program closeout\n- Plan next training milestones",
     "- Maintain proficiency schedule"}
};

static const char *gReqChecks[NUM_REQ_CHECK] = {
    "Night XC (>100 NM) completed",
    "10 night full-stop landings completed",
    "Solo long XC (150 NM / 3 stops / >50 NM leg) completed",
    "Towered solo (3 T/O + 3 full-stop) completed",
    "Knowledge test completed",
    "Stage Check 1 completed",
    "Stage Check 2 completed"
};

static const char *gReqHours[NUM_REQ_HOURS] = {
    "Total time (goal 40.0)",
    "Dual instruction (goal 20.0)",
    "Solo time (goal 10.0)",
    "Dual XC (goal 3.0)",
    "Night dual (goal 3.0)",
    "Instrument (goal 3.0)",
    "Checkride prep (goal 3.0)",
    "Solo XC time (goal 5.0)"
};

static const char *gFlashQ[NUM_FLASHCARDS] = {
    "What are the basic VFR weather minimums for Class E below 10,000 MSL?",
    "What are the basic VFR weather minimums for Class G day below 1,200 AGL?",
    "Define the IMSAFE checklist.",
    "Define the PAVE risk checklist.",
    "What does a solid magenta line on a sectional indicate?",
    "What does a dashed magenta line on a sectional indicate?",
    "What is the minimum safe cloud clearance in Class C?",
    "What is a runway incursion?",
    "What does METAR \"CLR\" or \"SKC\" mean?",
    "What does \"VRB\" indicate in a METAR wind group?",
    "What is a TAF?",
    "What is the purpose of a CTAF?",
    "Name a common night illusion and its risk.",
    "What is a stall?",
    "What increases stall speed?",
    "Lost procedures mnemonic?",
    "What is \"minimum fuel\"?",
    "What does \"expedite\" mean in ATC instructions?",
    "What is the 5 P's check?",
    "What is a normal traffic pattern altitude?",
    "What are the four stroke causes?",
    "What is the purpose of a run-up?",
    "What does \"AIM\" stand for?",
    "What is the standard rate turn?"
};

static const char *gFlashA[NUM_FLASHCARDS] = {
    "3 SM visibility, 500 below, 1,000 above, 2,000 horizontal.",
    "1 SM visibility and clear of clouds (day).",
    "Illness, Medication, Stress, Alcohol, Fatigue, Emotion/Eating.",
    "Pilot, Aircraft, enVironment, External pressures.",
    "Class C airspace boundary.",
    "Class D airspace boundary.",
    "3 SM, 500 below, 1,000 above, 2,000 horizontal.",
    "Any occurrence involving aircraft, vehicles, or people on a runway creating a hazard.",
    "Clear skies (no significant clouds).",
    "Wind direction variable.",
    "Terminal Aerodrome Forecast: expected weather for an airport.",
    "Common Traffic Advisory Frequency for non-towered airports.",
    "Black hole approach illusion: low visual cues can cause a low approach.",
    "An aerodynamic stall is loss of lift due to exceeding critical angle of attack.",
    "Increased weight, load factor, turbulence, or uncoordinated flight.",
    "Climb, Conserve, Communicate, Confess, Comply (varies by source).",
    "Advisory ATC phrase meaning you can't accept undue delays.",
    "Comply promptly; increase rate of action within safety.",
    "Plan, Plane, Pilot, Passengers, Programming.",
    "Typically 1,000 AGL (check local procedures).",
    "Airspeed, Configuration, Glidepath, Brakes.",
    "Verify engine health and ignition systems before takeoff.",
    "Aeronautical Information Manual.",
    "A turn at 3 degrees per second (360 deg in 2 minutes)."
};

typedef enum {
    SCREEN_MENU = 0,
    SCREEN_DAY,
    SCREEN_REQ,
    SCREEN_FLASH,
    SCREEN_ABOUT
} Screen;

typedef enum {
    TAB_FLIGHT = 0,
    TAB_GROUND,
    TAB_HW,
    TAB_NOTES
} Tab;

static PrintConsole gTopScreen;
static PrintConsole gBottomScreen;
static Keyboard *gKeyboard = NULL;

static SaveData gSave;
static bool gSaveEnabled = false;
static char gSavePath[64] = "/save.dat";

static Screen gScreen = SCREEN_MENU;
static Tab gTab = TAB_FLIGHT;
static int gMenuIndex = 0;
static int gReqIndex = 0;
static int gScroll[NUM_DAYS][TAB_COUNT];
static bool gEditing = false;
static int gFrame = 0;

static void consolePrintAt(int x, int y, const char *fmt, ...) {
    char buffer[128];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    iprintf("\x1b[%d;%dH%s", y, x, buffer);
}

static int countCompletedDays(void) {
    int count = 0;
    for (int i = 0; i < NUM_DAYS; ++i) {
        if ((gSave.dayFlags[i] & 0x7) == 0x7) {
            count++;
        }
    }
    return count;
}

static int countTaskChecks(void) {
    int total = 0;
    for (int i = 0; i < NUM_DAYS; ++i) {
        u8 flags = gSave.dayFlags[i];
        if (flags & 0x1) total++;
        if (flags & 0x2) total++;
        if (flags & 0x4) total++;
    }
    for (int i = 0; i < NUM_REQ_CHECK; ++i) {
        if (gSave.reqCheckbox[i]) total++;
    }
    return total;
}

static int computeStreak(void) {
    int streak = 0;
    for (int i = gSave.currentDay; i >= 0; --i) {
        if ((gSave.dayFlags[i] & 0x7) == 0x7) {
            streak++;
        } else {
            break;
        }
    }
    return streak;
}

static void renderHeader(void) {
    int completedDays = countCompletedDays();
    int taskChecks = countTaskChecks();
    int percent = (completedDays * 100) / NUM_DAYS;
    int filled = (percent * 20) / 100;
    char bar[21];
    for (int i = 0; i < 20; ++i) {
        bar[i] = (i < filled) ? '#' : '-';
    }
    bar[20] = '\0';

    consoleSelect(&gTopScreen);
    consoleClear();
    iprintf("+================================================+\n");
    iprintf("| PPL SYLLABUS DS // MISSION DAY %2d/56 // SAVE:%s |\n", gSave.currentDay + 1,
            gSaveEnabled ? "ON " : "OFF");
    iprintf("| ALT 3500  HDG 270  IAS 095  VSI +0  FUEL 26.5   |\n");
    iprintf("| XP  [%s] %3d%%  DAYS %02d/56 TASKS %03d |\n", bar, percent, completedDays, taskChecks);
    iprintf("| STREAK: %d                                     |\n", computeStreak());
    iprintf("+================================================+\n");
}

static int splitLines(const char *text, const char **starts, int *lens, int maxLines) {
    int count = 0;
    const char *lineStart = text;
    int len = 0;
    for (const char *p = text; ; ++p) {
        char c = *p;
        if (c == '\n' || c == '\0') {
            if (count < maxLines) {
                starts[count] = lineStart;
                lens[count] = len;
                count++;
            }
            if (c == '\0') {
                break;
            }
            lineStart = p + 1;
            len = 0;
        } else {
            len++;
        }
    }
    return count;
}

static void clampScrollValue(int *scroll, int lineCount) {
    int maxScroll = lineCount - VIEW_LINES;
    if (maxScroll < 0) maxScroll = 0;
    if (*scroll > maxScroll) *scroll = maxScroll;
    if (*scroll < 0) *scroll = 0;
}

static void renderTextWindow(const char *text, int startRow, int *scroll) {
    const char *lines[LINE_MAX];
    int lens[LINE_MAX];
    int lineCount = splitLines(text, lines, lens, LINE_MAX);
    clampScrollValue(scroll, lineCount);

    for (int i = 0; i < VIEW_LINES; ++i) {
        int idx = *scroll + i;
        consolePrintAt(0, startRow + i, "                                ");
        if (idx < lineCount) {
            iprintf("\x1b[%d;%dH%.*s", startRow + i, 0, lens[idx], lines[idx]);
        }
    }
}

static void saveData(void) {
    if (!gSaveEnabled) return;
    FILE *file = fopen(gSavePath, "wb");
    if (!file) return;
    fwrite(&gSave, sizeof(gSave), 1, file);
    fclose(file);
}

static void loadData(void) {
    if (!gSaveEnabled) {
        memset(&gSave, 0, sizeof(gSave));
        memcpy(gSave.magic, "PPLDS01", 7);
        gSave.version = 1;
        return;
    }
    FILE *file = fopen(gSavePath, "rb");
    if (!file) {
        memset(&gSave, 0, sizeof(gSave));
        memcpy(gSave.magic, "PPLDS01", 7);
        gSave.version = 1;
        return;
    }
    fread(&gSave, sizeof(gSave), 1, file);
    fclose(file);
    if (memcmp(gSave.magic, "PPLDS01", 7) != 0 || gSave.version != 1) {
        memset(&gSave, 0, sizeof(gSave));
        memcpy(gSave.magic, "PPLDS01", 7);
        gSave.version = 1;
    }
    if (gSave.currentDay >= NUM_DAYS) {
        gSave.currentDay = 0;
    }
    if (gSave.flashcardIndex >= NUM_FLASHCARDS) {
        gSave.flashcardIndex = 0;
    }
    gSave.flashcardReveal = gSave.flashcardReveal ? 1 : 0;
}

static void setupSave(void) {
    if (!fatInitDefault()) {
        gSaveEnabled = false;
        strcpy(gSavePath, "/save.dat");
        loadData();
        return;
    }

    gSaveEnabled = true;
    int result = mkdir("/PPLSYLLABUSDS", 0777);
    if (result == 0 || errno == EEXIST) {
        strcpy(gSavePath, "/PPLSYLLABUSDS/save.dat");
    } else {
        strcpy(gSavePath, "/save.dat");
    }
    loadData();
}

static void enterEditMode(void) {
    if (!gKeyboard) {
        gKeyboard = keyboardDemoInit();
    }
    keyboardSetVisible(gKeyboard, true);
    gEditing = true;
}

static void exitEditMode(void) {
    if (gKeyboard) {
        keyboardSetVisible(gKeyboard, false);
    }
    gEditing = false;
    saveData();
}

static bool touchInRect(touchPosition *touch, int x, int y, int w, int h) {
    return touch->px >= x && touch->px < x + w && touch->py >= y && touch->py < y + h;
}

static void renderMenu(void) {
    consoleSelect(&gBottomScreen);
    consoleClear();

    int blink = (gFrame / 30) % 2;
    const char *cursor = blink ? ">>" : "  ";

    consolePrintAt(0, 1, "MAIN MENU");

    const char *labels[4] = {
        "DAY BROWSER",
        "REQUIREMENTS",
        "FLASHCARDS",
        "ABOUT"
    };

    int y = 4;
    for (int i = 0; i < 4; ++i) {
        consolePrintAt(0, y, "%s +----------------------+", (gMenuIndex == i) ? cursor : "  ");
        consolePrintAt(0, y + 1, "%s |    %-14s   |", (gMenuIndex == i) ? cursor : "  ", labels[i]);
        consolePrintAt(0, y + 2, "%s +----------------------+", (gMenuIndex == i) ? cursor : "  ");
        y += 4;
    }
    consolePrintAt(0, 20, "START: jump to last day");
    consolePrintAt(0, 21, "SELECT: save now");
}

static void renderTabs(void) {
    const char *tabNames[TAB_COUNT] = {"FLIGHT", "GROUND", "HW", "NOTES"};
    int x = 0;
    for (int i = 0; i < TAB_COUNT; ++i) {
        if ((Tab)i == gTab) {
            consolePrintAt(x, 21, ">%s<", tabNames[i]);
        } else {
            consolePrintAt(x, 21, " %s ", tabNames[i]);
        }
        x += 8;
    }
}

static void renderDayBrowser(void) {
    consoleSelect(&gBottomScreen);
    consoleClear();

    DayData day = gDays[gSave.currentDay];
    consolePrintAt(0, 0, "DAY %d/56: %s", gSave.currentDay + 1, day.label);

    if (gTab == TAB_FLIGHT || gTab == TAB_GROUND || gTab == TAB_HW) {
        u8 bit = (gTab == TAB_FLIGHT) ? 0x1 : (gTab == TAB_GROUND) ? 0x2 : 0x4;
        consolePrintAt(0, 2, "[%c] Completed", (gSave.dayFlags[gSave.currentDay] & bit) ? 'X' : ' ');
    } else {
        consolePrintAt(0, 2, "[EDIT] Notes (A or tap)");
    }

    const char *content = "";
    if (gTab == TAB_FLIGHT) {
        content = day.flight;
        if (content[0] == '\0') content = "(No flight scheduled)";
    } else if (gTab == TAB_GROUND) {
        content = day.ground;
        if (content[0] == '\0') content = "(No ground scheduled)";
    } else if (gTab == TAB_HW) {
        content = day.homework;
        if (content[0] == '\0') content = "(No homework scheduled)";
    } else {
        content = gSave.notes[gSave.currentDay];
        if (content[0] == '\0') content = "(No notes yet)";
    }

    renderTextWindow(content, 4, &gScroll[gSave.currentDay][gTab]);
    renderTabs();
    consolePrintAt(0, 22, "L/R day  UP/DN scroll  B back");
}

static void renderRequirements(void) {
    consoleSelect(&gBottomScreen);
    consoleClear();
    int blink = (gFrame / 30) % 2;
    const char *cursor = blink ? ">>" : "  ";

    consolePrintAt(0, 0, "REQUIREMENTS TRACKER");

    int line = 2;
    for (int i = 0; i < NUM_REQ_CHECK; ++i) {
        consolePrintAt(0, line, "%s[%c] %s", (gReqIndex == i) ? cursor : "  ",
                       gSave.reqCheckbox[i] ? 'X' : ' ', gReqChecks[i]);
        line++;
    }

    line++;
    for (int i = 0; i < NUM_REQ_HOURS; ++i) {
        int idx = NUM_REQ_CHECK + i;
        int value = gSave.reqHours[i];
        consolePrintAt(0, line, "%s%4d.%d  %s", (gReqIndex == idx) ? cursor : "  ",
                       value / 10, value % 10, gReqHours[i]);
        line++;
    }
    consolePrintAt(0, 21, "LEFT/RIGHT adjust  A toggle");
    consolePrintAt(0, 22, "B back  SELECT save");
}

static void renderFlashcards(void) {
    consoleSelect(&gBottomScreen);
    consoleClear();

    int idx = gSave.flashcardIndex % NUM_FLASHCARDS;
    consolePrintAt(0, 0, "FLASHCARDS %d/%d", idx + 1, NUM_FLASHCARDS);
    consolePrintAt(0, 1, "A reveal  L/R prev/next");

    int qScroll = 0;
    int aScroll = 0;
    renderTextWindow(gFlashQ[idx], 3, &qScroll);
    if (gSave.flashcardReveal) {
        consolePrintAt(0, 12, "ANSWER:");
        renderTextWindow(gFlashA[idx], 13, &aScroll);
    } else {
        consolePrintAt(0, 12, "ANSWER: (hidden)");
    }
    consolePrintAt(0, 22, "B back");
}

static void renderAbout(void) {
    consoleSelect(&gBottomScreen);
    consoleClear();
    consolePrintAt(0, 0, "ABOUT / HOW TO USE");
    consolePrintAt(0, 2, "PPLSYLLABUSDS keeps a 56-day");
    consolePrintAt(0, 3, "syllabus, requirements, and");
    consolePrintAt(0, 4, "flashcards.");
    consolePrintAt(0, 6, "SAVE:%s", gSaveEnabled ? "ON" : "OFF");
    consolePrintAt(0, 8, "SELECT saves manually.");
    consolePrintAt(0, 9, "Notes use the on-screen kb.");
    consolePrintAt(0, 11, "B back");
}

static void renderEditOverlay(void) {
    consoleSelect(&gTopScreen);
    consolePrintAt(0, 7, "EDITING NOTES (START to save)");
    const char *content = gSave.notes[gSave.currentDay];
    int previewScroll = 0;
    renderTextWindow(content[0] ? content : "(typing...)", 9, &previewScroll);
}

static void handleMenuInput(int keysDown, int keysHeld, touchPosition *touch) {
    if (keysDown & KEY_UP) {
        gMenuIndex = (gMenuIndex + 3) % 4;
    }
    if (keysDown & KEY_DOWN) {
        gMenuIndex = (gMenuIndex + 1) % 4;
    }
    if (keysDown & KEY_A) {
        gScreen = (Screen)(gMenuIndex + 1);
        saveData();
    }
    if (keysDown & KEY_START) {
        gScreen = SCREEN_DAY;
        saveData();
    }

    if (keysHeld & KEY_TOUCH) {
        if (touchInRect(touch, 0, 32, 256, 96)) {
            int idx = (touch->py - 32) / 24;
            if (idx >= 0 && idx < 4) {
                gMenuIndex = idx;
                if (keysDown & KEY_TOUCH) {
                    gScreen = (Screen)(gMenuIndex + 1);
                    saveData();
                }
            }
        }
    }
}

static void handleDayInput(int keysDown, int keysHeld, touchPosition *touch) {
    if (gEditing) {
        int c = keyboardUpdate();
        size_t len = strlen(gSave.notes[gSave.currentDay]);
        if (c > 0 && isprint(c)) {
            if (len < NOTE_MAX - 1) {
                gSave.notes[gSave.currentDay][len] = (char)c;
                gSave.notes[gSave.currentDay][len + 1] = '\0';
            }
        } else if (c == 8) {
            if (len > 0) {
                gSave.notes[gSave.currentDay][len - 1] = '\0';
            }
        }
        if (keysDown & KEY_START) {
            exitEditMode();
        }
        if (keysDown & KEY_B) {
            exitEditMode();
        }
        return;
    }

    if (keysDown & KEY_L) {
        if (gSave.currentDay > 0) gSave.currentDay--;
    }
    if (keysDown & KEY_R) {
        if (gSave.currentDay < NUM_DAYS - 1) gSave.currentDay++;
    }
    if (keysDown & KEY_UP) {
        gScroll[gSave.currentDay][gTab]--;
    }
    if (keysDown & KEY_DOWN) {
        gScroll[gSave.currentDay][gTab]++;
    }

    if (keysDown & KEY_A) {
        if (gTab == TAB_NOTES) {
            enterEditMode();
            return;
        }
        u8 bit = (gTab == TAB_FLIGHT) ? 0x1 : (gTab == TAB_GROUND) ? 0x2 : 0x4;
        gSave.dayFlags[gSave.currentDay] ^= bit;
    }

    if (keysDown & KEY_B) {
        gScreen = SCREEN_MENU;
        saveData();
        return;
    }

    if (keysHeld & KEY_TOUCH) {
        if (touchInRect(touch, 0, 168, 256, 24)) {
            int tabIdx = touch->px / 64;
            if (tabIdx >= 0 && tabIdx < TAB_COUNT) {
                gTab = (Tab)tabIdx;
            }
        }

        if (gTab == TAB_NOTES) {
            if (touchInRect(touch, 0, 16, 256, 24)) {
                if (keysDown & KEY_TOUCH) {
                    enterEditMode();
                }
            }
        } else {
            if (touchInRect(touch, 0, 16, 120, 24)) {
                if (keysDown & KEY_TOUCH) {
                    u8 bit = (gTab == TAB_FLIGHT) ? 0x1 : (gTab == TAB_GROUND) ? 0x2 : 0x4;
                    gSave.dayFlags[gSave.currentDay] ^= bit;
                }
            }
        }
    }
}

static void handleRequirementsInput(int keysDown) {
    int totalItems = NUM_REQ_CHECK + NUM_REQ_HOURS;
    if (keysDown & KEY_UP) {
        gReqIndex = (gReqIndex + totalItems - 1) % totalItems;
    }
    if (keysDown & KEY_DOWN) {
        gReqIndex = (gReqIndex + 1) % totalItems;
    }
    if (gReqIndex < NUM_REQ_CHECK) {
        if (keysDown & KEY_A) {
            gSave.reqCheckbox[gReqIndex] = !gSave.reqCheckbox[gReqIndex];
        }
    } else {
        int hourIdx = gReqIndex - NUM_REQ_CHECK;
        if (keysDown & KEY_LEFT) {
            if (gSave.reqHours[hourIdx] > 0) gSave.reqHours[hourIdx]--;
        }
        if (keysDown & KEY_RIGHT) {
            gSave.reqHours[hourIdx]++;
        }
    }
    if (keysDown & KEY_B) {
        gScreen = SCREEN_MENU;
        saveData();
    }
}

static void handleFlashcardInput(int keysDown) {
    if (keysDown & KEY_L) {
        if (gSave.flashcardIndex == 0) {
            gSave.flashcardIndex = NUM_FLASHCARDS - 1;
        } else {
            gSave.flashcardIndex--;
        }
        gSave.flashcardReveal = 0;
    }
    if (keysDown & KEY_R) {
        gSave.flashcardIndex = (gSave.flashcardIndex + 1) % NUM_FLASHCARDS;
        gSave.flashcardReveal = 0;
    }
    if (keysDown & KEY_A) {
        gSave.flashcardReveal = !gSave.flashcardReveal;
    }
    if (keysDown & KEY_B) {
        gScreen = SCREEN_MENU;
        saveData();
    }
}

static void handleAboutInput(int keysDown) {
    if (keysDown & KEY_B) {
        gScreen = SCREEN_MENU;
        saveData();
    }
}

int main(void) {
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&gTopScreen, 0, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&gBottomScreen, 0, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

    setupSave();

    while (pmMainLoop()) {
        scanKeys();
        int keysDown = keysDown();
        int keysHeld = keysHeld();
        touchPosition touch;
        touchRead(&touch);

        if (keysDown & KEY_SELECT) {
            saveData();
        }

        renderHeader();

        switch (gScreen) {
            case SCREEN_MENU:
                handleMenuInput(keysDown, keysHeld, &touch);
                renderMenu();
                break;
            case SCREEN_DAY:
                handleDayInput(keysDown, keysHeld, &touch);
                renderDayBrowser();
                if (gEditing) {
                    renderEditOverlay();
                }
                break;
            case SCREEN_REQ:
                handleRequirementsInput(keysDown);
                renderRequirements();
                break;
            case SCREEN_FLASH:
                handleFlashcardInput(keysDown);
                renderFlashcards();
                break;
            case SCREEN_ABOUT:
                handleAboutInput(keysDown);
                renderAbout();
                break;
        }

        gFrame++;
        swiWaitForVBlank();
    }

    return 0;
}
