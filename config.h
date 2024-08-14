#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int snap      = 32;       /* snap pixel */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static const int showbar            = 0;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "JetBrains Mono:size=11", "JoyPixels:pixelsize=11:antialias=true:autohint=true"};
static const char dmenufont[]       = "JetBrains Mono:size=11";
static char normbgcolor[]           = "#222222";
static char normbordercolor[]       = "#444444";
static char normfgcolor[]           = "#bbbbbb";
static char selfgcolor[]            = "#eeeeee";
static char selbordercolor[]        = "#005577";
static char selbgcolor[]            = "#005577";
static const unsigned int baralpha = 0xd0;
static const char *colors[][3] = {
       /*               fg           bg           border   */
       [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
       [SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
};
static const unsigned int alphas[][3]      = {
	/*               fg      bg     */
	[SchemeNorm] = { OPAQUE, baralpha },
	[SchemeSel]  = { OPAQUE, baralpha },
};

static const char *const autostart[] = {
       "sh", "-c", "newlook", NULL,
       "dwmblocks", NULL,
       "fcitx5", NULL,
       "redshift", "-l", "53:18", "-t", "6500:4000", NULL,
       "picom", "--config", "/etc/xdg/picom.conf.example", NULL,
       NULL /* terminate */
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class                instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
	{ "St",                 NULL,     NULL,           0,         0,          1,           0,        -1 },
	{ NULL,                 NULL,     "Event Tester", 0,         0,          0,           1,        -1 }, /* xev */
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define PrintScreenDWM  0x0000ff61
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *j4cmd[] = { "j4-dmenu-desktop", "--dmenu",  "dmenu -i -fn 'Jetbrains Mono-11'"};
static const char *termcmd[]  = { "st", NULL };
static const char *upvol[] = {"pactl", "set-sink-volume", "0", "+5%", NULL};
static const char *downvol[] = {"pactl", "set-sink-volume", "0", "-5%", NULL};
static const char *mutevol[] = {"pactl", "set-sink-mute", "0", "toggle", NULL};
static const char *screenshotcmd[] = {"scrot", "/home/shiren/screenshots/%Y-%m-%d-%H:%M:%S.png", NULL};
static const char *brightnessdown[] = {"light", "-U", "5", NULL};
static const char *brightnessup[] = {"light", "-A", "5", NULL};
static const char *play[] = {"playerctl", "play-pause", NULL};
static const char *previous[] = {"playerctl", "previous", NULL};
static const char *next[] = {"playerctl", "next", NULL};

static Key keys[] = {
	/* modifier             key                       function        argument */
	{ MODKEY,               XK_l,                     setmfact,       {.f = +0.05} },
	{ MODKEY,               XK_k,                     focusstack,     {.i = -1 } },
 	{ MODKEY,               XK_j,                     focusstack,     {.i = +1 } },
	{ MODKEY,               XK_h,                     setmfact,       {.f = -0.05} },
	{ MODKEY,               XK_g,                     togglebar,      {0} },
	{ MODKEY,               XK_f,	                  zoom,           {0} },
	{ MODKEY,               XK_d,                     incnmaster,     {.i = -1 } },
	{ MODKEY,               XK_s,                     incnmaster,     {.i = +1 } },
	{ MODKEY,		XK_q,                     killclient,     {0} },
	{ MODKEY,               XK_w,                     setlayout,      {.v = &layouts[0]} },
	{ MODKEY,               XK_e,                     setlayout,      {.v = &layouts[1]} },
	{ MODKEY,               XK_r,                     setlayout,      {.v = &layouts[2]} },
	{ MODKEY|ShiftMask,     XK_r,  	                  togglefloating, {0} },
	{ MODKEY|ShiftMask,     XK_f,                     togglefullscr,  {0} },
	{ MODKEY,               XK_t,  	                  setlayout,      {0} },
	{ MODKEY,               XK_space,                 spawn,          {.v = j4cmd } },
	{ MODKEY,               XK_Return,                spawn,          {.v = termcmd } },
        { MODKEY,               XK_Cyrillic_de,           setmfact,       {.f = +0.05} },
        { MODKEY,               XK_Cyrillic_el,           focusstack,     {.i = -1 } },
        { MODKEY,               XK_Cyrillic_o,            focusstack,     {.i = +1 } },
        { MODKEY,               XK_Cyrillic_er,           setmfact,       {.f = -0.05} },
        { MODKEY,               XK_Cyrillic_pe,           togglebar,      {0} },
        { MODKEY,               XK_Cyrillic_a,            zoom,           {0} },
        { MODKEY,               XK_Cyrillic_ve,           incnmaster,     {.i = -1 } },
        { MODKEY,               XK_Cyrillic_yeru,         incnmaster,     {.i = +1 } },
        { MODKEY,               XK_Cyrillic_shorti,       killclient,     {0} },
        { MODKEY,               XK_Cyrillic_tse,          setlayout,      {.v = &layouts[0]} },
        { MODKEY,               XK_Cyrillic_u,            setlayout,      {.v = &layouts[1]} },
        { MODKEY,               XK_Cyrillic_ka,           setlayout,      {.v = &layouts[2]} },
        { MODKEY,               XK_Cyrillic_en,           setlayout,      {.v = &layouts[3]} },
        { MODKEY|ShiftMask,     XK_Cyrillic_ka,           togglefloating, {0} },
        { MODKEY,               XK_Cyrillic_ie,           setlayout,      {0} },
        { MODKEY,               XK_Tab,                   view,           {0} },
	{ MODKEY,               XK_0,                     view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,     XK_0,                     tag,            {.ui = ~0 } },
	{ MODKEY,               XK_comma,                 focusmon,       {.i = -1 } },
	{ MODKEY,               XK_period,                focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,     XK_comma,                 tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,     XK_period,                tagmon,         {.i = +1 } },
	{ MODKEY,               XK_F5,                    xrdb,           {.v = NULL } },
        { 0,                    0x1008ff13,               spawn,          {.v = upvol} },
        { 0,                    0x1008ff11,               spawn,          {.v = downvol} },
        { 0,                    0x1008ff12,               spawn,          {.v = mutevol} },
        { 0,                    PrintScreenDWM,           spawn,          {.v = screenshotcmd} },
        { 0,                    XF86XK_MonBrightnessDown, spawn,          {.v = brightnessdown} },
        { 0,                    XF86XK_MonBrightnessUp,   spawn,          {.v = brightnessup} },
        { 0,                    XF86XK_AudioPlay,         spawn,          {.v = play} },
        { 0,                    XF86XK_AudioPrev,         spawn,          {.v = previous} },
        { 0,                    XF86XK_AudioNext,         spawn,          {.v = next} },
	TAGKEYS(                XK_1,                      0)
	TAGKEYS(                XK_2,                      1)
	TAGKEYS(                XK_3,                      2)
	TAGKEYS(                XK_4,                      3)
	{ MODKEY|ShiftMask,	XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
