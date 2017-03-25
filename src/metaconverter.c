// metaconverter.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>        /* for strncpy  */
#include <ctype.h>

#define METADIR ""
#define METAXMLDIR "xmls"
#define METACSVDIR "csvs"
#define CONVERT_START_DATE 105

#define VERSION_NUMBER  0x3636
/* emaster: 192 bytes
offset   length   desc
0        2        0x36,0x36 version
2        1        x in Fx.DAT
3        8        unknown
11       14       stock symbol ends with byte 0
25       7        unknown
32       16       stock name ends with byte 0
48       16       unknown
64       4        first date YYMMDD
68       4        unknown
72       4        last date  YYMMDD
76       50       unknown
126      4        first date long YYYYMMDD
130      1        unknown
131      5        last dividend paid
135      4        last dividend adjustment rate
139      53       unknown
*/
#define EMASTER_FX                 2
#define EMASTER_SYMBOL             11
#define EMASTER_NAME               32
#define EMASTER_FIRST_DATE         64
#define EMASTER_LAST_DATE          72
#define EMASTER_FIRST_DATE_LONG    126
#define EMASTER_LAST_DIVIDEND      131
#define EMASTER_LAST_DIVIDEND_ADJ  135

/* master: 52 bytes
offset   length   desc
0        1        x in Fx.DAT
1        6        unknown
7        16       stock name ends with byte 0
23       2        unknown
25       4        first date YYMMDD
29       4        last date YYMMDD
33       3        unknown
36       14       stock symbol ends with byte 0
51       3        unknown
*/

#define MASTER_FX                 0
#define MASTER_SYMBOL             36
#define MASTER_NAME               7
#define MASTER_FIRST_DATE         25
#define MASTER_LAST_DATE          29


/*
xmaster: 150 bytes
offset   length   desc
0        1        unknown
1        15       stock symbol ends with byte 0
16       46       stock name ends with byte 0
62       3?       'D' maybe update type
65       2        n in Fn.MWD
67       13       unknown
80       4        end date e.g. 19981125
84       20       unknown
104      4        start date
108      4        start date
112      4        unknown
116      4        end date
120      30       unknown
*/

#define XMASTER_MARK            0
#define XMASTER_SYMBOL          1
#define XMASTER_NAME            16
#define XMASTER_D               62 
#define XMASTER_FN              65
#define XMASTER_END_DATE_1      80
#define XMASTER_START_DATE_1    104
#define XMASTER_START_DATE_2    108
#define XMASTER_END_DATE_2      116

/*
Fx.DAT or Fn.MWD
offset   length   desc
0        4        date YYMMDD
4        4        open
8        4        high
12       4        low
16       4        close
20       4        volume
24       4        open interest
*/


int process_master( void );
int process_emaster( void );
int process_xmaster( void );
int _fmsbintoieee(float *src4, float *dest4);
int process_fdata(const char* symbol, FILE* fdatafile);

int main(int argc, char *argv[])
//int _tmain(int argc, _TCHAR* argv[])
{

	process_master();

    //process_emaster();

	process_xmaster();

	return EXIT_SUCCESS;
}

int process_emaster( void )
{
	int end = 0;
	unsigned int NoRead;
	unsigned char Buffer[192];

	char outstr[100];

	sprintf(outstr, "%sEMASTER", METADIR);

	FILE* emasterfile = fopen(outstr, "rb");

	if (!emasterfile)
	{
		return -1;
	}

	while (!end)
	{
		if (!feof(emasterfile))
		{
			NoRead = fread(Buffer, 1, 192, emasterfile);
			/*check the version number*/
			if ((NoRead == 192)
				&& (Buffer[0] == 0x36)
				&& (Buffer[1] == 0x36)
				)
			{

				unsigned char temp = Buffer[EMASTER_FIRST_DATE + 3];
				unsigned int dene2 = 0x49852518;
				//int date, year, month, day;
				float out2 = 0.0;
				float out3 = 0.0, out4 = 0.0;

				char datfilename[256];
				char symbol[15];
				FILE* datfile;
				Buffer[EMASTER_FIRST_DATE + 3] = (temp << 4) + (temp >> 4);
				temp = Buffer[EMASTER_LAST_DATE + 3];
				Buffer[EMASTER_LAST_DATE + 3] = (temp << 4) + (temp >> 4);
				_fmsbintoieee((float*)&Buffer[EMASTER_FIRST_DATE], &out2);
				_fmsbintoieee((float*)&Buffer[EMASTER_LAST_DATE], &out3);
				printf("F%d.DAT: %s, %s, %f, %f\n", Buffer[EMASTER_FX],
					&Buffer[EMASTER_SYMBOL],
					&Buffer[EMASTER_NAME],
					out2, out3);
				sprintf(datfilename, "%sF%d.DAT", METADIR, Buffer[EMASTER_FX]);
				sprintf(symbol, "%s", &Buffer[EMASTER_SYMBOL]);
				datfile = fopen(datfilename, "rb");
				if (datfile)
				{
					process_fdata(symbol, datfile);
					fclose(datfile);
				}
			}
		}
		else
		{
			end++;
		}
	}
	fclose(emasterfile);
	return 0;
}

int process_master( void )
{
	int end = 0;
	unsigned int NoRead;
	unsigned char Buffer[53];

	char outstr[100];

	sprintf(outstr, "%sMASTER", METADIR);

	FILE* masterfile = fopen(outstr, "rb");

	if (!masterfile)
	{
		return -1;
	}

	while (!end)
	{
		if (!feof(masterfile))
		{
			NoRead = fread(Buffer, 1, 53, masterfile);

			if ((NoRead == 53)
				)
			{
				//float out2;
				//float out3, out4;

				char datfilename[256];
				char symbol[256];
				FILE* datfile;
				printf("F%d.DAT: %s, %s\n", Buffer[MASTER_FX],
					&Buffer[MASTER_SYMBOL],
					&Buffer[MASTER_NAME]);

				sprintf(datfilename, "%sF%d.DAT", METADIR, Buffer[MASTER_FX]);
				sprintf(symbol, "%s", &Buffer[MASTER_SYMBOL]);
				if (symbol[4] == 0x20)
				{
					symbol[4] = '\0';
				}
				if (symbol[5] == 0x20)
				{
					symbol[5] = '\0';
				}
				symbol[6] = '\0';
				datfile = fopen(datfilename, "rb");
				if (datfile)
				{
					process_fdata( symbol, datfile );
					fclose(datfile);
				}
			}
		}
		else
		{
			end++;
		}
	}
	fclose(masterfile);
	return 0;
}

int process_xmaster( void )
{
	int end = 0;
	unsigned int NoRead;
	unsigned char Buffer[150];

	char outstr[100];

	sprintf(outstr, "%sXMASTER", METADIR);

	FILE* xmasterfile = fopen(outstr, "rb");

	if (!xmasterfile)
	{
		return -1;
	}

	while (!feof(xmasterfile))
	{
		NoRead = fread(Buffer, 1, 150, xmasterfile);
		/*check the version number*/
		if ((NoRead == 150)
			&& (Buffer[0] == 0x01)
			)
		{
			unsigned char temp = Buffer[EMASTER_FIRST_DATE + 3];

			unsigned int out3, out4;

			char datfilename[256];
			char symbol[15];
			FILE* datfile;

			out3 = (Buffer[XMASTER_START_DATE_1] << 24) + \
					(Buffer[XMASTER_START_DATE_1 + 1] << 16) + \
					(Buffer[XMASTER_START_DATE_1 + 2] << 8) + \
					Buffer[XMASTER_START_DATE_1 + 3];

			out4 = (Buffer[XMASTER_END_DATE_2] << 24) + \
					(Buffer[XMASTER_END_DATE_2 + 1] << 16) + \
					(Buffer[XMASTER_END_DATE_2 + 2] << 8) + \
					Buffer[XMASTER_END_DATE_2 + 3];

			printf("F%d.MWD: %s, %s, %d, %d\n", (Buffer[XMASTER_FN + 1] << 8) + Buffer[XMASTER_FN],
				&Buffer[XMASTER_SYMBOL],
				&Buffer[XMASTER_NAME],
				out3, out4);

			sprintf(datfilename, "%sF%d.MWD", METADIR, (Buffer[XMASTER_FN + 1] << 8) + Buffer[XMASTER_FN]);
			sprintf(symbol, "%s", &Buffer[XMASTER_SYMBOL]);
			datfile = fopen(datfilename, "rb");
				
			if (datfile)
			{
				process_fdata( symbol, datfile );
				fclose(datfile);
			}
		}		
	}
	fclose(xmasterfile);
	return 0;
}

int process_fdata( const char* symbol, FILE* fdatafile )
{
	int end = 0;
	unsigned int NoRead;
	unsigned char Buffer[24];
	char filename[256];
	int i;

	int arridx = 0;
	float date, open, close, low, high, amount;

	FILE* xmlfile;
	FILE* csvfile;

	NoRead = fread(Buffer, 1, 24, fdatafile);
	memset(filename, 0, 256);
	sprintf(filename, "%s/%s.xml", METAXMLDIR, symbol);
	for (i = 0; filename[i]; i++)
	{
		filename[i] = tolower(filename[i]);
	}

	xmlfile = fopen(filename, "w+");
	if (!xmlfile)
	{
		return -1;
	}

	sprintf(filename, "%s/%s.csv", METACSVDIR, symbol);
	for (i = 0; filename[i]; i++)
	{
		filename[i] = tolower(filename[i]);
	}

	csvfile = fopen(filename, "w+");
	if (!csvfile)
	{
		return -1;
	}
	fprintf(xmlfile, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
	fprintf(xmlfile, "<stock name=\"%s\">\n", symbol);
	
	fprintf(csvfile, "Date,Open,Low,High,Close,Volume\n");
	while (!feof(fdatafile))
	{
		NoRead = fread(Buffer, 1, 24, fdatafile);
		/*check the version number*/
		if (NoRead == 24)
		{

			_fmsbintoieee((float*)&Buffer[0], &date);
			_fmsbintoieee((float*)&Buffer[4], &open);
			_fmsbintoieee((float*)&Buffer[8], &high);
			_fmsbintoieee((float*)&Buffer[12], &low);
			_fmsbintoieee((float*)&Buffer[16], &close);
			_fmsbintoieee((float*)&Buffer[20], &amount);

			if ((((int)date) / 10000)<CONVERT_START_DATE)
			{
				continue;
			}

			fprintf(xmlfile, "<date id=\"%d\">", (int)date);
			fprintf(xmlfile, "<open>%.4f</open>", open);
			fprintf(xmlfile, "<close>%.4f</close>", close);
			fprintf(xmlfile, "<high>%.4f</high>", high);
			fprintf(xmlfile, "<low>%.4f</low>", low);
			fprintf(xmlfile, "<amount>%ld</amount>", ((int)(amount*close)));
			fprintf(xmlfile, "</date>\n");

			fprintf(csvfile,"%d,%.4f,%.4f,%.4f,%.4f,%.2f\n",(int)date,open,low,high,close,amount);

			arridx++;
		}
	}

	fprintf(xmlfile, "</stock>\n");
	fclose(xmlfile);
	fclose(csvfile);
	return 0;
}

int _fmsbintoieee(float *src4, float *dest4)
{
	unsigned char *msbin = (unsigned char *)src4;
	unsigned char *ieee = (unsigned char *)dest4;
	unsigned char sign = 0x00;
	unsigned char ieee_exp = 0x00;
	int i;

	/* MS Binary Format                         */
	/* byte order =>    m3 | m2 | m1 | exponent */
	/* m1 is most significant byte => sbbb|bbbb */
	/* m3 is the least significant byte         */
	/*      m = mantissa byte                   */
	/*      s = sign bit                        */
	/*      b = bit                             */

	sign = msbin[2] & 0x80;      /* 1000|0000b  */

	/* IEEE Single Precision Float Format       */
	/*    m3        m2        m1     exponent   */
	/* mmmm|mmmm mmmm|mmmm emmm|mmmm seee|eeee  */
	/*          s = sign bit                    */
	/*          e = exponent bit                */
	/*          m = mantissa bit                */

	for (i = 0; i<4; i++) ieee[i] = 0;

	/* any msbin w/ exponent of zero = zero */
	if (msbin[3] == 0) return 0;

	ieee[3] |= sign;

	/* MBF is bias 128 and IEEE is bias 127. ALSO, MBF places   */
	/* the decimal point before the assumed bit, while          */
	/* IEEE places the decimal point after the assumed bit.     */

	ieee_exp = msbin[3] - 2;    /* actually, msbin[3]-1-128+127 */

	/* the first 7 bits of the exponent in ieee[3] */
	ieee[3] |= ieee_exp >> 1;

	/* the one remaining bit in first bin of ieee[2] */
	ieee[2] |= ieee_exp << 7;

	/* 0111|1111b : mask out the msbin sign bit */
	ieee[2] |= msbin[2] & 0x7f;

	ieee[1] = msbin[1];
	ieee[0] = msbin[0];
	return 0;
}

