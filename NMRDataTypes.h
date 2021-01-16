#pragma once

#include "stdafx.h"

/****************************************************************************
* SECTION NAME:       Static Instrument header definitions
* SECTION CONTENTS:
*                The following structures defined here are used in the Header
*                AIR_CONTROL, ALARM_CONTROL, DEC_CONTROL,  FILT_CONTROL,
*                F1_CONTROL,  GAIN_CONTROL,  LOCK_CONTROL, SPOIL_CONTROL,
*                VT_CONTROL
****************************************************************************/

/* These defines should correspond to the menu position of these items in
	the stop_shim_menu menu in Dshim.c */
#define GAIN	2
#define TXPWR	3
#define FREQ	4
#define OFFSET	5
#define PHASE	6
#define SWIDTH	7
#define SRATE	8

#define LOCK_SWEEP_FRACT 2

/*----------------------------------------------------------------------
 *       Air Control Structure for air commands: spin, eject, bodyair & gas.
 *         For VT see VT_CONTROL.
 */

typedef struct airtag {
	long eject_flag;	/* 392 Eject on/off 1/0                          */
	long spin_state;	/* 396 Spin on/off 1/0                           */
	long spin_rate;		/* 400 Spin rate in hertz                        */
	long spin_val;		/* 404 In dac units 0 to 2047                    */
	long spin_hi_alarm;	/* 408 high spin rate alarm limit                */
	long spin_lo_alarm; /* 412 low spin rate alarm limit                 */
	long body_flag;		/* 416 Body air on/off 1/0                       */
	long gas_flag;		/* 420 Gas N2/air 1/0                            */
	long vt_flag;		/* 424 VT air on/off 1/0                         */
	long vac_flag;		/* 428 Vacuum pump on/off 1/0                    */
} AIR_CONTROL;			// 10 DWORDs, 40 bytes

/*----------------------------------------------------------------------
 *       Alarm Control Structure for vtalarm command, etc.
 */

/* Define vtalarm enable/on states, bit-significant. */
#define VT_ALARM_ON		1
#define SPIN_ALARM_ON	2
#define LOCK_ALARM_ON	4

/* Define vtalarm actions, bit-significant. */
#define RESUME_ACTION		0x01
#define ABORT_ACTION		0x02
#define DEC_DIS_ACTION		0x04
#define CSHIM_STP_ACTION	0x08
#define GAS_OFF_ACTION		0x10
#define HEAT_OFF_ACTION		0x20
#define EJECT_ACTION		0x40

typedef struct alarmtag {
	long on_flag;			/* 432 */
	long lock_lost;			/* 436 */
	long vt_high;			/* 440 */
	long vt_low;			/* 444 */
	long vt_gas;			/* 448 */
	long spin_high;			/* 452 */
	long spin_low;			/* 456 */
	long unused1;			/* 460 */
} ALARM_CONTROL;

/*---------------------------------------------------------------------
 *       Filter Control Structure for filter command.
 */

#define FILT_BES_MODE	2
#define FILT_BUT_MODE	1
#define FILT_DIR_MODE	0

typedef struct filtertag {
	float sw_fraction;	/* 720 Cutoff freq as fraction of sw.             */
	long freq;			/* 724 Cutoff freq (3 db) of filter in hertz.     */
	long mode;			/* 728 Mode bessel/butterworth/off.               */
	long preamp;		/* 732 Preamp filter on/off 1/0.                  */
	long locktrap;		/* 736 Lock Trap on/off 1/0.                      */
	long freq_override;	/* 740 Used for freq_override on/off 1/0 Rel 4.4. */
	long unused1;		/* 744 padding */
	long unused2;		/* 748 */
} FILT_CONTROL;

/*---------------------------------------------------------------------
 *       Frequency Control Structure for f1 command.
 *
 *	NOTE: Power values (pwr_val, hi_pwr_val, lo_pwr_val) depend
 *			on System type, either:
 *				attenuation value from 0 - 63, or
 *				DAC value from 0 - 2047.
 */
 
/*	Define bits used in hd->f1.unused1 for Preamp & Selection module.   */
/*	Bits are defined so that (unused1 = 0) gives default settings.      */
#define	F1_HOMO_DISABLE	1	/* Default: homo enabled, Level 1 line #5.  */
#define	F1_SEL_CROSSED	2	/* Default: selection straight, Level 1 #6. */
#define F1_CAL_ON		4	/* Default: cal off, Level 1 line #7.       */

typedef struct f1tag {
	double freq;		/* 464 Frequency in Mhz.                        */
	double ref_freq;	/* 472 Reference frequency for offset calc      */
	double nu_offset;	/* 480 nucleus offset frequency in ppm          */
	long hi_pwr_flag;	/* 488 High/Low 1/0                             */
	long pwr_val;		/* 492 For Shell & scripts only.                */
	long hi_pwr_val;	/* 496 For SI Task.                    	        */
	long lo_pwr_val;	/* 500 For SI Task.                    	        */
	long cpma_flag;     /* 504 CPMA On/Off 1/0                 	        */
	char nucleus[8];	/* 508 Isotope identification                   */
	long lo_sideband;	/* 516 Low SideBand setting                     */
	long mod_pad;		/* 520 Modulator attenuator in/out 1/0.         */
	long unused1;		/* 524 Starting Rel. 5.1: for Preamp & Select.  */
} F1_CONTROL;

/*---------------------------------------------------------------------
 *       Frequency Control Structure for f2 & f3 commands.
 */

/* Defines for "blank_ctrl" */
#define B_FOLLOW_HB '2'
#define B_ENABLE    '1'
#define B_DISABLE   '0'

/* Defines for "hetero_flag" */
#define HETERO      '2'
#define HOMO        '1'
#define H_OFF       '0'

typedef struct dectag {
	double freq;		/* 528 Frequency in Mhz.                         */
	double ref_freq;	/* 536 Reference frequency for offset calc       */
	double nu_offset;	/* 544 nucleus offset frequency in ppm           */
	float rate;			/* 552 Program rate in hertz                     */
	long on_flag;		/* 556 On/off 1/0                                */
	long atten_state;	/* 560 State 2/State 1 2/1                       */
	long pwr_val;		/* 564 Value from 0 thru 82                      */
	long cw_flag;		/* 568 CW/Program 1/0                            */
	long rom_program;	/* 572 ROM Program number, 0 thru 3              */
	long phase_offset;	/* 576 Phase Offset, 0 90 180 270                */
	long phase_tweak;	/* 580 Phase Tweak, -2048 thru 2047              */
	long time1;			/* 584 Pulse Width in microseconds, 0 - 255      */
 	long time2;			/* 588 Post Delay in microseconds, 0 - 60        */
	long hw_divider;	/* 592 HW frequency divider setting -- RAM only. */
	long l2_pwr_val;	/* 596 Used for 1280 F3 L2 pwr_val               */
	char blank_ctrl;	/* 600 Preamp DX Blanking Ctrl(not used by SI):  */
						/*      follow_hb/enable/disable '2'/'1'/'0'     */
	char blanking;		/* 601 Preamp DX Blanking (used by SI Task):     */
						/*      enable/disable '1'/'0'                   */
	char cpma_flag;		/* 602 Used for cpma_flag on/off '1'/'0' Rel 4.4.*/
	char hetero_flag;	/* 603 Hetero/Homo/off '2'/'1'/'0'               */
	char nucleus[8];	/* 604 Isotope identification                    */
	long mod_pad;		/* 612 Modulator attenuator in/out 1/0.          */
	long unused1;		/* 616 padding */
	long unused2;		/* 620 */
} DEC_CONTROL;

/*---------------------------------------------------------------------
 *       Gain Control Structure for gain command.
 */

typedef struct gaintag {
	float gain_val;		/* 752 Value of gain, 1 thru 10,000.            */
	long mode;			/* 756 Cmd mode, when = 1, user controls attn's.*/
	long if_on;			/* 760 IF Attenuator In/Out 1/0.                */
	long rf_on;			/* 764 RF Attenuator In/Out 1/0.                */
	long af_on;			/* 768 AF Amplifier On/Off 1/0.                 */
	long unused1;		/* 772 padding */
} GAIN_CONTROL;

/*---------------------------------------------------------------------
 *       Lock Control Structure for lock command.
 */

typedef struct locktag {
	double freq;		/* 776 Freq in Mhz.                              */
	double ref_freq;	/* 784 Reference frequency for offset calc       */
	double nu_offset;	/* 792 nucleus offset frequency in ppm           */
	float delta;		/* 800 The change in Freq or Field during sweep. */
	long state;			/* 804 On/off 1/0                                */
	long mode;			/* 808 Fast/slow/standby 2/1/0                   */
	long gain;			/* 812 In dac units 0 to 2047.                   */
	long power;			/* 816 In dac units 0 to 2047.                   */
	long offset;		/* 820 In dac units 0 to 2047.                   */
	long phase;			/* 824 In dac units 0 to 2047.                   */
	long sweep;			/* 828 Frequency/field/off 2/1/0                 */
	long type;			/* 832 Triangular/sawtooth 1/0                   */
	long buff_size;		/* 836 Must be >= 4                              */
	long dwell;			/* 840 In microseconds.                          */
	long lo_alarm;		/* 844 Lower lock level alarm value.             */
	char nucleus[8];	/* 848 Isotope identification                    */
} LOCK_CONTROL;

/*---------------------------------------------------------------------
 *       Homospoil Control Structure for spoil command.
 */

typedef struct spoiltag {
	long x_val;	/* 856 Delta amount of DAC units to change X Coarse Shim DAC.   */
	long y_val;	/* 860 Delta amount of DAC units to change Y Coarse Shim DAC.   */
	long z_val;	/* 864 Delta amount of DAC units to change Z1 Coarse Shim DAC.  */
	long unused1; /* 868 */
} SPOIL_CONTROL;

/*---------------------------------------------------------------------
 *       VT Control Structure for vt command.
 */

typedef struct vttag {
	float	set_point;		/* 872 Desired temperature in degrees C */
	float	hi_alarm;		/* 876 High alarm temperature           */
	float	lo_alarm;		/* 880 Low alarm temperature            */
	long		state;		/* 884 VT on/off 1/0                    */
	long		unused1;	/* 888 padding */
	long		unused2;	/* 892 */
} VT_CONTROL;

/****************************************************************************
* SECTION NAME:       Top level sane header definition
* SECTION CONTENTS:
*	These are the parameters that form the sane structset header, hdsane.
****************************************************************************/

typedef struct hdtag        /*  sane header structset, hdsane*/
{							/*  Offset, in bytes from beginning of structure */
	double cf_current;		/*    0 config freq including offset (Mhz) */
	double cf_base;		    /*    8 config freq that puts TMS in proton spect */
	double spec_width[4];   /*   16 Spectral width (hz) */
	double dim_freq[4];	    /*   48 Frequency for corresponding dimension */
	double dwell_time[4];	/*   80 Dwell time (sec) */
	double at;				/*  112 Acquisition time (sec) */
	double de;				/*  120 Acq delay (usec) */
	double rg;				/*  128 Receiver gate pulse */
	double de_dw_ratio; 	/*  136 just what it says... */
	float fov[4];			/*  144 Field of View (mm) for images */
	float gain_corr;		/*  160 ADC Gain correction */
	float phase_corr;		/*  164 ADC Phase correction */
	float iphase;			/*  168 ADC Initial Phase */
	float incphase;         /*  172 ADC Incremental Phase */
	float of[4];			/*  176 spectrum offset (Hz) */
	float phasea[4];		/*  192 Total 0th order phase correction applied */
	float phaseb[4];		/*  208 Total 1st order phase correction applied */
	float data_maximum;	    /*  224 Data maximum - used in display routines */
	float data_minimum;	    /*  228 Data minimum - used in display routines */
	float dis_floor;		/*  232 Display floor in percent */
	long dim[4];			/*  236 size in each dim (complex points) */
	long ftflag[4];		    /*  252 0=>for fid; 1=>spectrum */
	long spec_rev[4];	    /*  268 spectrum reverse req'd during processing */
	long block_size;		/*  284 size of fid in complex points */
	long groupnum;		    /*  288 number of view groups */
	long groupcur;		    /*  292 current view group */
	long dimension;		    /*  296 data set dimension (1..4) */
	long hetero_flag;	    /*  300 flag for hetero nuclear experiment */
	long sample_number;     /*  304 sample changer sample number */
	long adcmemsize;		/*  308 ADC Memory size */
	long phase_cycle;	    /*  312 number of acq per phase cycle */
	long na;				/*  316 Number of acquisitions */
	long nac;			    /*  320 Number of acquisitions start count */
	long nb;				/*  324 Number of Blocks */
	long nbc;				/*  328 Number of Blocks start count */
	long nc;				/*  332 additional limits */
	long ncc;				/*  336 and counters....  */
	long nd;				/*  340 */
	long ndc;				/*  344 */
	long nq;				/*  348 Number of echos */
	long nqc;				/*  352 Number of echos count */
	long ns;				/*  356 */
	long nsc;				/*  360 sequence counter */
	long dg;				/*  364 number of sequences w/o triggers */
	long dgc;				/*  368 counter 0 .. dg */
	long acqmode;			/*  372 0="raw" or 1="alt" */
	long channel;			/*  376 ADC Channel ('A' or 'B') */
	long trigsrc;			/*  380 ADC Trigger Source ('I' or 'X') */
	long extdw;				/*  384 External Dwell 0=off, 1=on */
	long unused1;			/*  388 size = 98 wds THRU this point */
	AIR_CONTROL		air;	/*  392 10 wds */
	ALARM_CONTROL	alarm;	/*  432 8 wds */
	F1_CONTROL		f1;		/*  464 16 wds */
	DEC_CONTROL		f2;		/*  528 24 wds */
	DEC_CONTROL		f3;		/*  624 24 wds */
	FILT_CONTROL	filter;	/*  720 8 wds */
	GAIN_CONTROL	gain;	/*  752 6 wds */
	LOCK_CONTROL	lock;	/*  776 20 wds */
	SPOIL_CONTROL	spoil;	/*  856 4 wds */
	VT_CONTROL		vt;		/*  872 6 wds */
} HD;					//  896 total bytes!

typedef struct decouplertag {
	double freq, offset;
	float rate;
	bool cw;
	int power, program;
} DECOUPLER;

typedef struct specpartag {
	double at, de, df, dof, dw, f1freq, of, rg, sf, sw, temp;
	double dunused1, dunused2, dunused3, dunused4;
	float filterfreq, gain, pha, phb, phc, set_temp;
	float apoValue, qca, qcg;
	float funused1, funused2, funused3, integralconst;
	bool f1highpower, filterlocktrap, filterpreamp, ftflag, ssb, tempset;
	bool baseline, polybaseline, magnitude, dclevel, isLoren, isPPM, level, filtercomp;
	bool bunused1, bunused2, bunused3, bintegralconst;
	int acqmode, blocksize, dg, f1power, groupnum;
	int filtermode, lock_gain, lock_power, ns;
	int lrshift;
	int iunused1, iunused2, iunused3, groupcur;
	DECOUPLER f2, f3;
} SPECTRUMPARAMETERS;

typedef struct regiontag {
	float value;
	int start, stop;
} REGION;

enum eLeafType {base, root, folder, fid, spectrum, netnmrfile, net12nmrfile};
enum eDimension {simple, arrayed, twod};
enum eVendor {bruker, agilent};
enum eLocation {shared, served};

union Leaf {
	LPARAM l;
	struct
	{
		int drivenumber	: 8;
		int leaftype	: 8;
		int dimension	: 4;
		int vendor		: 4;
		int location	: 4;
	} s;
};

enum DisplayColors {
	imfidColor = 0,
	refidColor = 1,
	spectColor = 2,
	integColor = 3,
	intehColor = 4,
	axis_Color = 5,
	titleColor = 6,
	parboColor = 7,
	parheColor = 8,
	paramColor = 9,
	peaklColor = 10,
	peakhColor = 11,
	peaktColor = 12,
	basenColor = 13,
	basefColor = 14,
	crossColor = 15,
	zoomcColor = 16,
	backgColor = 17
};