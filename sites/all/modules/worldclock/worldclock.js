<!--

var timerId;
var delay=1000;
var out_dst_str;

//--------------------------------------------------------------------
function ChangeFormat(myobj)
{
	target = document.getElementById('worldclock_timestr_div');

	sel = myobj.selectedIndex;
	if(myobj[sel].value == 'custom') {
		// Custom format is selected - show target div block
		target.style.visibility = "visible";
		target.style.height = "";	// undefine
		target.style.overflow = "hidden";
	}
	else {
		target.style.visibility = "hidden";
		target.style.height = 0; 
		target.style.overflow = "hidden";
	}
}

//--------------------------------------------------------------------
function DisplayClock(maxnum, time_str, show_dst, dst_str, locations)
{
	// In: maxnum   	1 to 8 (number of locations to display)
	//     time_str		time format string (eg: 'm/d h:i:s A')
	//     show_dst		0=No, 1=Yes
	//	   dst_str		DST symbol string (eg: '*' or 'DST')
	//     locations	array - locations[1], locations[2], locations[3] ...
	//
	var loc, diff, dst, i;

	for(i = 1 ; i <= maxnum ; i++) {
		loc = locations[i];
    if (!loc) {
      continue;
    }
		// convert string parameters to integer
		for(j = 0; j <= 11; j++) {
			switch(j) {
				case 1:			// loc_name (= string)
					continue;
				case 2: 		// loc_diff (= float)
					loc[j] = parseFloat(loc[j]); 
					break;
				default:		// others (= integer)
					loc[j] = parseInt(loc[j]);
					break;
			}
		}
		// ---- loc array ----
		// [0] 	loc_no
		// [1] 	loc_name
		// [2] 	loc_diff 
		// [3] 	loc_dst
		// [4]	loc_smon
		// [5]	loc_sth
		// [6]	loc_sdow
		// [7]	loc_stime
		// [8]	loc_emon
		// [9]	loc_eth
		// [10]	loc_edow
		// [11]	loc_etime
		if(loc[0] == 0) continue;
		diff = loc[2];
		dst  = loc[3];
		out_dst_str = "";
		if(dst == 1) {		// the location has DST
			if(IsDST(loc)) {
				diff++;
				if(show_dst) {
					out_dst_str = " " + dst_str;
				}
			}
		}
		idname = "worldclock" + i;
		document.getElementById(idname).innerHTML 
			= GenerateTimeString(diff, time_str) + out_dst_str;
	}
	timerId = setTimeout("DisplayClock(Drupal.settings.maxnum, Drupal.settings.time_str, Drupal.settings.show_dst, Drupal.settings.dst_str, Drupal.settings.locations)", delay);
}

//--------------------------------------------------------------------
function IsDST(loc)
{
	// NOTE: this function is called only for the countries 
	//       which has the summer time (DST)
	// ---- loc array ----
	// [0] 	loc_no
	// [1] 	loc_name
	// [2] 	loc_diff 
	// [3] 	loc_dst
	// [4]	loc_smon
	// [5]	loc_sth
	// [6]	loc_sdow
	// [7]	loc_stime
	// [8]	loc_emon
	// [9]	loc_eth
	// [10]	loc_edow
	// [11]	loc_etime
	var localDate, localTime, localYear, startYear, endYear;
	var tzDiff, tzOffset, dstStartGMT, dstEndGMT, nowGMT;

	// calculate time difference from GMT (in minute)
	tzDiff = loc[2] * 60; 

	localDate = new Date();
	localTime = localDate.getTime();	// in msec
	localYear = localDate.getFullYear();
	startYear = endYear = localYear; // Usually DST start/end date apply to the same year
	
	// time difference between GMT and local time in minute
	tzOffset = localDate.getTimezoneOffset();
  nowGMT = localTime + (tzOffset + tzDiff) * 60 * 1000;

	if(loc[4] > loc[8]) {
		// e.g DST starts Nov 1 and ends March 30 of the next year
		// for countries in southern half such as Australia, New Zealand 
    dstEndGMT = GetDstTime(localYear, loc[8], loc[9], loc[10], loc[11]);
    // If now is before DST end, consider DST period from previous year to current
    // year
    if (nowGMT < dstEndGMT ) {
		  startYear--;
    } // otherwise consider DST period from cur year to next one
    else {
      endYear++;
    }
	}


	dstStartGMT = GetDstTime(startYear, loc[4], loc[5], loc[6], loc[7]);
	dstEndGMT = GetDstTime(endYear, loc[8], loc[9], loc[10], loc[11]);


	if((dstStartGMT <= nowGMT) && (nowGMT < dstEndGMT)) {
		return 1;
	}
	return 0;
}

//--------------------------------------------------------------------
function GenerateTimeString(loc_diff, time_str)
{
	var dtNowLocal = new Date;
	var dtNow = new Date;
	var curHour, curMin, curSec, curMon, curDate, curDow, wk;

    var today = new Date();
    var year = today.getFullYear();

	dtNow.setTime(dtNowLocal.getTime() + (dtNowLocal.getTimezoneOffset() + (loc_diff * 60)) * 60 * 1000);

	curMon  = dtNow.getMonth() + 1;
	curDate = dtNow.getDate();
	curDow  = dtNow.getDay();		// 0(SUN) - 6(SAT)
	curHour = dtNow.getHours();		// 0 - 23
	curMin  = dtNow.getMinutes();
	curSec  = dtNow.getSeconds();
	strOut  = "";

	for(i = 0 ; i < time_str.length ; i++) {
		c = time_str.charAt(i);
		switch(c) {
			case 'd':	// date 01 - 31
				wk = curDate;
				if(wk < 10) wk = "0" + wk;
				strOut += wk;
				break;
			case 'j':	// date 1 - 31
				strOut += curDate;
				break;
			case 'M':
				strOut += Drupal.settings.smon[curMon-1];
				break;
			case 'm':	// month 01 - 12
				wk = curMon;
				if(wk < 10) wk = "0" + wk;
				strOut += wk;
				break;
			case 'n':	// month 1 - 12
				strOut += curMon;
				break;
			case 'Y':	// year (XXXX)
				strOut += dtNow.getFullYear();
				break;
			case 'y':	// year (XX)
				wk = dtNow.getYear();
				if(wk > 100) wk -= 100;
				if(wk < 10) wk = "0" + wk;
				strOut += wk;
				break;
			case 'D':	// DOW (Sun - Sat)
				strOut += Drupal.settings.sdow[curDow];
				break;
			case 'a':	// am or pm
				if(curHour < 12) strOut += "am";
				else strOut += "pm";
				break;
			case 'A':	// AM or PM
				if(curHour < 12) strOut += "AM";
				else strOut += "PM";
				break;
			case 'g':	// hour (12hour) 1 - 12
				wk = curHour;
				if(wk >= 12) wk -= 12;
				strOut += wk;
				break;
			case 'h':	// hour (12hour) 01 - 12
				wk = curHour;
				if(wk >= 12) wk -= 12;
				if(wk < 10) wk = "0" + wk;
				strOut += wk;
				break;
			case 'G':	// hour (24hour) 0 - 23
				strOut += curHour;
				break;
			case 'H':	// hour (24hour) 00 - 23
				wk = curHour;
				if(wk < 10) wk = "0" + wk;
				strOut += wk;
				break;
			case 'i':	// minutes 00 - 59
				wk = curMin;
				if(wk < 10) wk = "0" + wk;
				strOut += wk;
				break;
			case 's':	// second 00 - 59
				wk = curSec;
				if(wk < 10) wk = "0" + wk;
				strOut += wk;
				break;
			// special
			case 'T':	// GMT +- difference
				if(0 <= loc_diff) strOut += "GMT+" + loc_diff;
				else strOut += "GMT" + loc_diff;
				break;
			case 'l':	// DOW (Sunday - Saturday)
			  strOut += Drupal.settings.ldow[curDow];
			  break;
			case 'N':	// DOW (1 - 7) ISO-8601
			  strOut += (curDow + 1) % 7;
			  break;
			case 'w':	// DOW (0 - 6)
			  strOut += curDow;
			  break;
			case 'S':	// Nth suffix
			case 'W':	// week
			case 'F':	// Month (January - December)
  			strOut += Drupal.settings.lmon[curMon-1];
				break;
			case 't':	// number of days in the month (28 - 31)
			case 'L':	// leap year ? (0=no, 1=yes)
			case 'o':	// leap year 
			case 'B':	// swatch internet time
			case 'u':	// msec
			case 'e':
			case 'I':
			case 'O':
			case 'P':
			case 'T':
			case 'Z':
			case 'c':
			case 'r':
			case 'U':
				// not supported
				break;
			case ' ':
				strOut += "&nbsp;";
				break;
			default:
				strOut += c;	// just copy
				break;
		}
	}
	return(strOut);
}

//--------------------------------------------------------------------
function GetDstTime(year, mon, th, dow, tim) 
{
	var dst_day;
	var hit_cnt;
	var i;
	var numDays = new Array(0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31);

	if(th == 9) {	// last
		dst_day = new Date(year, mon-1, numDays[mon], tim, 0, 0); 
		// decrement date starting from the last day of the month
		for (i = numDays[mon]; i >= 1; i--) {
			dst_day.setDate(i);
			if (dst_day.getDay() == dow) {
				break;	// hit!
			}
		}
	}
	else {	// 1st, 2nd, 3rd, 4th..
		dst_day = new Date(year, mon-1, 1, tim, 0, 0); 
		// increment date starting from the first day of the month
		for (i = 1, hit_cnt = 0; i <= 31; i++) {
			dst_day.setDate(i);
			if (dst_day.getDay() == dow) {
				hit_cnt++;	// hit!
				if(hit_cnt == th) {
					break;
				}
			}
		}
	}
	return dst_day.getTime();
}

// -->
