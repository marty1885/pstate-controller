/*
 * This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <string>
#include <algorithm>
#include <chrono>
#include <thread>
using namespace std;

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//From http://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
double getCurrentValue()
{
	static uint64_t lastTotalUser, lastTotalUserLow
		,lastTotalSys, lastTotalIdle;
	static bool firstRun = true;
	if(firstRun == true)
	{
		FILE* file = fopen("/proc/stat", "r");
		fscanf(file, "cpu %lu %lu %lu %lu", &lastTotalUser, &lastTotalUserLow,
			&lastTotalSys, &lastTotalIdle);
		fclose(file);
		firstRun = false;
	}
	double percent;
	FILE* file;
	uint64_t totalUser, totalUserLow, totalSys, totalIdle, total;

	file = fopen("/proc/stat", "r");
	fscanf(file, "cpu %lu %lu %lu %lu", &totalUser, &totalUserLow,
		&totalSys, &totalIdle);
	fclose(file);

	if (totalUser < lastTotalUser || totalUserLow < lastTotalUserLow ||
		totalSys < lastTotalSys || totalIdle < lastTotalIdle){
		//Overflow detection. Just skip this value.
		percent = -1.0;
	}
	else{
		total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) +
			(totalSys - lastTotalSys);
		percent = total;
		total += (totalIdle - lastTotalIdle);
		percent /= total;
		percent *= 100;
	}

	lastTotalUser = totalUser;
	lastTotalUserLow = totalUserLow;
	lastTotalSys = totalSys;
	lastTotalIdle = totalIdle;

	return percent;
}

int main()
{
	string intelPstateFolder = "/sys/devices/system/cpu/intel_pstate";
	const string echoCommand = "echo ";

	if(intelPstateFolder[intelPstateFolder.size()] != '/')
		intelPstateFolder += "/";

	const float scalerMin = 22;
	const float scalerMax = 100;
	const float scalingScaler = 1.7;
	const float activateThreshold = 35;
	const int updateTimeMili = 1500;

	getCurrentValue();//ignore the first reasult. It's bad
	while(1)
	{
		this_thread::sleep_for(chrono::milliseconds(updateTimeMili));
		float usage = getCurrentValue();
		float limitScaler = usage*scalingScaler;
		if(limitScaler > scalerMax)
			limitScaler = scalerMax;
		else if(limitScaler < scalerMin)
			limitScaler = scalerMin;
		if(limitScaler < activateThreshold)
			limitScaler = scalerMin;
		int scalerInt = (int)limitScaler;
		char value[5];
		sprintf(value,"%d ",scalerInt);
		system((echoCommand + value + " > "
			+ intelPstateFolder + "max_perf_pct").c_str());

		//cout << usage << "\t" << limitScaler << endl;

	}
	return 0;
}
