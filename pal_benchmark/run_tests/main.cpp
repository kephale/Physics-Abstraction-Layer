#include <stdio.h>
#include <string>
#include <vector>
#include <math.h>

#ifdef _WIN32
	#include <windows.h>
#endif

using namespace std;
/*
	PAL Test Collection
    Copyright (C) 2007  Adrian Boeing

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

vector<string> g_engines;

float ReadFrictionResult(const char *prefix,const char *engine, const float t) {
	float result;
	FILE *fin = 0;
	char buf[256];
	sprintf(buf,"%s_%s_%4.3f.txt",prefix,engine,t);
	while (!fin) {
		fin = fopen(buf,"r");
		sleep(20);
	}
	fscanf(fin,"%f",&result);
	fclose(fin);
	sleep(20);
	unlink(buf);
	return result;
}


float ReadLinksResult(const char *prefix, const char *engine, const int num) {
	float result;
	FILE *fin = 0;
	char buf[256];
	sprintf(buf,"%s_%s_%d.txt",prefix,engine,num);
	while (!fin) {
		fin = fopen(buf,"r");
		sleep(20);
	}
	fscanf(fin,"%f",&result);
	fclose(fin);
	sleep(20);
	unlink(buf);
	return result;
}

char *ReadFile(const char *name, const char *engine){
	FILE *fin =0;
	char buf[256];
	sprintf(buf,"%s_%s.txt",name,engine);
	while (!fin) {
		fin = fopen(buf,"r");
		sleep(20);
	}
	fseek(fin,0,SEEK_END);
	int size = ftell(fin);
	fseek(fin,0,SEEK_SET);
	char *data = new char[size+1];
	fread(data,sizeof(char),size,fin);
	data[size]=0;
	fclose(fin);
	sleep(50);
	unlink(buf);
	return data;
}

FILE *Open(const char *filename) {
	FILE *fin =0;
	while (!fin) {
		fin = fopen(filename,"r");
		sleep(20);
	}
	return fin;
}

void run_drop_test() {
	FILE *fout = 0;
	//FILE *fout_time = 0;
	char cmd[4096];
	fout=fopen("drop_results.csv","w");
	fprintf(fout,"Ideal,");
	float tmax=5;
	float td=0.01f;
	float g=-9.8;
	for (float t=0;t<tmax;t+=td) {
		fprintf(fout,"%f,",0.5*g*t*t);
	}
	fprintf(fout,"\nEuler,");
	float v=0;
	float s=0;
	for (float t=0;t<tmax;t+=td) {
		s+=v*td;
		v+=g*td;
		fprintf(fout,"%f,",s);
	}
	fprintf(fout,"\nSymplectic Euler,");
	v=0;
	s=0;
	for (float t=0;t<tmax;t+=td) {
		v+=g*td;
		s+=v*td;
		fprintf(fout,"%f,",s);
	}
	fprintf(fout,"\nEuler 2nd order,"); //midpoint?
	v=0;
	s=0;
	for (float t=0;t<tmax;t+=td) {
		s+=v*td+0.5*g*td*td;
		v+=g*td;
		fprintf(fout,"%f,",s);
	}
	fprintf(fout,"\nVerlet,");
	v=0;
	float sip1=0;
	float si=0;
	float sim1=0;

	for (float t=0;t<tmax;t+=td) {
		sip1= si+(si-sim1)+g*td*td;
		fprintf(fout,"%f,",si);
		sim1=si;
		si=sip1;
	}

	fprintf(fout,"\nDrag (smooth),");
	//http://en.wikipedia.org/wiki/Drag_(physics)
	float ga = fabs(g);
	float r = 1; //radius
	float p = 1.2; //density at room temp
	float A = M_PI*r*r; //area
	//float C = 0.1; //smooth sphere http://aerodyn.org/Drag/tables.html
	float C = 0.005; //Subsonic Aircraft Wing, minimum 
	const float m=1;
	float k1 = sqrt((2*m*ga)/(p*A*C));
	float k2 = sqrt((ga*p*C*A)/(2*m));
	for (float t=0;t<tmax;t+=td) {
		s=k1/k2*log(cosh(k2*t));//integral of tanh eq
		fprintf(fout,"%f,",-s);
	}
	fprintf(fout,"\n");

	for (int j=0;j<g_engines.size();j++) {
		printf("DROP:testing %s\n",g_engines[j].c_str());
		sprintf(cmd,"test_drop.exe n %s %f",g_engines[j].c_str(),tmax);
		system(cmd);
		fprintf(fout,"%s,",g_engines[j].c_str());
		fprintf(fout,"%s\n",ReadFile("drop",g_engines[j].c_str()));
		fflush(fout);
	}
	fclose(fout);

}

void run_restitution_test() {
	FILE *fout = 0;
	//FILE *fout_time = 0;
	char cmd[4096];
		int j;
	std::vector<std::string> results1;
	std::vector<std::string> results2;
	std::vector<std::string> results3;
	for (j=0;j<g_engines.size();j++) {
		printf("RESTITUTION:testing %s\n",g_engines[j].c_str());
		sprintf(cmd,"test_restitution.exe n %s 3",g_engines[j].c_str());
		system(cmd);
		std::string result=std::string("restitution_path_") + g_engines[j] + ".txt";
		FILE *fin = Open(result.c_str());
		char linebuf[32768];
		fscanf(fin,"%s\n",linebuf);
		results1.push_back(linebuf);
		fscanf(fin,"%s\n",linebuf);
		results2.push_back(linebuf);
		fscanf(fin,"%s\n",linebuf);
		results3.push_back(linebuf);
		fclose(fin);
		unlink(result.c_str());
	}

	fout = fopen("restitution_path_results.csv","w");

	//float g=-9.8;
	//float a=g;
	float r=0.1f;
	for (j=0;j<g_engines.size();j++) {
		fprintf(fout,"%s,%s\n",g_engines[j].c_str(),results1[j].c_str());
	}
	fprintf(fout,"\n");
	r=0.5f;
	for (j=0;j<g_engines.size();j++) {
		fprintf(fout,"%s,%s\n",g_engines[j].c_str(),results2[j].c_str());
	}
	fprintf(fout,"\n");
	/*
	r=1.0f;
	fprintf(fout,"Ideal:");
	for (float t=0;t<3;t+=0.01f) {
		float p = 1-(0.5*a*t*t);
		if (p<0) a*=-r;
	}
*/
	for (j=0;j<g_engines.size();j++) {
		fprintf(fout,"%s,%s\n",g_engines[j].c_str(),results3[j].c_str());
	}
	fclose(fout);

}

void run_friction_test() {
	FILE *fout = 0;
	FILE *fout_accel = 0;
	char cmd[4096];
	fout = fopen("friction_results.csv","w");
	if (fout == 0) {
		printf("Could not open results file, probably because it is being used by another application.\n");
		return;
	}

	fout_accel = fopen("friction_accel_results.csv","w");

	fprintf(fout,"x:,");
	fprintf(fout_accel,"theta:,");
//#define MAX M_PI/8
//#define STEP 0.02

#define MAX M_PI/4
#define STEP 0.05
	for (float t=0;t<MAX;t+=STEP) {
		fprintf(fout,"%f,",tan(t));
		fprintf(fout_accel,"%f,",t);
	}
	fprintf(fout,"\n");
	fprintf(fout_accel,"\n");
	for (float f=STEP*2;f<MAX;f+=STEP*2) {
		fprintf(fout		,"friction coefficient:,%f\n",f);
		fprintf(fout_accel	,"friction coefficient:,%f\n",f);
		for (int j=0;j<g_engines.size();j++) {
			fprintf(fout		,"%s,",g_engines[j].c_str());
			fprintf(fout_accel	,"%s,",g_engines[j].c_str());
			for (float t=0;t<MAX;t+=STEP) {
				float r;
				printf("FRICTION:testing %s %4.3f\n",g_engines[j].c_str(),t);
				//		sprintf(cmd,"test_friction.exe n %s %4.3f 0.6 0.7",g_engines[j].c_str(),t);
				sprintf(cmd,"test_friction.exe n %s %4.3f 1.5 1.7 %f %f",g_engines[j].c_str(),t,f,f-STEP*2);
				system(cmd);
				r=ReadFrictionResult("friction",g_engines[j].c_str(),t);
				fprintf(fout,"%f,",r);
				fflush(fout);

				r=ReadFrictionResult("friction_accel",g_engines[j].c_str(),t);
				fprintf(fout_accel,"%f,",r);
				fflush(fout_accel);

			}
			fprintf(fout,"\n");
			fprintf(fout_accel,"\n");
		}
	}
	fclose(fout);
	fclose(fout_accel);
}


void run_collision_test() {
	FILE *fout = 0;
	//FILE *fout_time = 0;
	char cmd[4096];
	for (int i=0;i<4;i++) {
		int step_hz=100;
		switch (i) {
			case 0:
				step_hz = 100;
				break;
			case 1:
				step_hz = 60;
				break;
			case 2:
				step_hz = 30;
				break;
			case 3:
				step_hz = 15;
				break;

		}
	sprintf(cmd,"collision_results_%.3d.csv",step_hz);
	fout=fopen(cmd,"w");
	for (int j=0;j<g_engines.size();j++) {
		printf("COLLISION:testing %s\n",g_engines[j].c_str());
		sprintf(cmd,"test_collision.exe n %s 2 %f",g_engines[j].c_str(),1/(float)step_hz);
		system(cmd);
		fprintf(fout,"%s,",g_engines[j].c_str());
		fprintf(fout,"%s\n",ReadFile("collision",g_engines[j].c_str()));
		fflush(fout);
	}
//	fprintf(fout,"Comments: Only bullet; jiggle, newton and true axis pass this test. ODE fails completely; Novodex next worst; Tokamak come close to working.\n");
	fclose(fout);
	}
}

void run_momentum_test() {
	FILE *fout = 0;
	//FILE *fout_time = 0;
	char cmd[4096];
		fout=fopen("momentum_results.csv","w");
	for (int j=0;j<g_engines.size();j++) {
		printf("MOMENTUM:testing %s\n",g_engines[j].c_str());
		sprintf(cmd,"test_momentum.exe n %s 10",g_engines[j].c_str());
		system(cmd);
		fprintf(fout,"%s,",g_engines[j].c_str());
		fprintf(fout,"%s\n",ReadFile("momentum",g_engines[j].c_str()));
		fflush(fout);
	}
	fprintf(fout,"Comments: Tokamak shows full errors; others that tend out have dampening factors to improve the simualtions 'stability'\n");
	fclose(fout);

}


void run_links_test() {
	FILE *fout = 0;
	FILE *fout_time = 0;
	char cmd[4096];
	fout = fopen("links_results.csv","w");
	if (fout == 0) {
		printf("Could not open results file, probably because it is being used by another application.\n");
		return;
	}
	fout_time = fopen("links_time_results.csv","w");
	if (fout_time == 0) {
		printf("Could not open results file, probably because it is being used by another application.\n");
		return;
	}
	for (int j=0;j<g_engines.size();j++) {
		fprintf(fout,"%s,",g_engines[j].c_str());
		fprintf(fout_time,"%s,",g_engines[j].c_str());
	for (int i=3;i<13;i++) {
		float r;
		printf("LINKS:testing %s %d\n",g_engines[j].c_str(),i);
		sprintf(cmd,"test_links.exe n %s %d 20",g_engines[j].c_str(),i);
		system(cmd);
		r=ReadLinksResult("links",g_engines[j].c_str(),i);
		fprintf(fout,"%f,",r);
		fflush(fout);
		r=ReadLinksResult("links_time",g_engines[j].c_str(),i);
		fprintf(fout_time,"%f,",r);
		fflush(fout_time);
	}
	fprintf(fout,"\n");
	fprintf(fout_time,"\n");
	}
	fclose(fout);
	fclose(fout_time);
}


void run_stack_test(const char *active, int max_stack_size = 21, int step_rate = 100) {
	float step_size = 1/(float)step_rate;
	//FILE *fout = 0;
	FILE *fout_time = 0;
	char cmd[4096];
	sprintf(cmd,"stack_time_results_%s_%.3d.csv",active,step_rate);
	fout_time = fopen(cmd,"w");
	if (fout_time == 0) {
		printf("Could not open results file, probably because it is being used by another application.\n");
		return;
	}
	for (int j=0;j<g_engines.size();j++) {
		fprintf(fout_time,"%s,",g_engines[j].c_str());
	for (int i=3;i<max_stack_size;i++) {
		float r;
		printf("STACK:testing %s %d\n",g_engines[j].c_str(),i);
		sprintf(cmd,"test_stack.exe n %s %d 10 %s %f",g_engines[j].c_str(),i,active,step_size);
		system(cmd);
		r=ReadLinksResult("stack_time",g_engines[j].c_str(),i);
		fprintf(fout_time,"%f,",r);
		fflush(fout_time);
	}
	fprintf(fout_time,"\n");
	}
	fclose(fout_time);
#if 0
	fout_time = fopen("stack_time_results_active.csv","w");
	if (fout_time == 0) {
		printf("Could not open results file, probably because it is being used by another application.\n");
		return;
	}
	for (int j=0;j<g_engines.size();j++) {
		fprintf(fout_time,"%s,",g_engines[j].c_str());
	for (int i=3;i<max_stack_size;i++) {
		float r;
		printf("STACK:testing %s %d\n",g_engines[j].c_str(),i);
		sprintf(cmd,"test_stack.exe n %s %d 10 a 0.01",g_engines[j].c_str(),i);
		system(cmd);
		r=ReadLinksResult("stack_time",g_engines[j].c_str(),i);
		fprintf(fout_time,"%f,",r);
		fflush(fout_time);
	}
	fprintf(fout_time,"\n");
	}
	fclose(fout_time);
#endif
}

void run_stack_tests() {
#if 0
	run_stack_test("active",21,100);
	run_stack_test("sleep",21,100);
#else
	run_stack_test("active",10,100);
	run_stack_test("active",10,60);
	run_stack_test("active",10,30);
#endif
}

int main(int argc, char *argv[]) {
	g_engines.push_back("Bullet");
//	g_engines.push_back("Dynamechs"); //experimental
	g_engines.push_back("Jiggle");
	g_engines.push_back("Newton");
	g_engines.push_back("Novodex");
	g_engines.push_back("ODE");
	g_engines.push_back("Tokamak");
//	g_engines.push_back("OpenTissue"); //experimental
	g_engines.push_back("TrueAxis");
//*/

	run_collision_test();
	run_drop_test();
//	run_friction_test();
	run_links_test();
//	run_momentum_test();
	run_restitution_test();
	run_stack_tests();
}
