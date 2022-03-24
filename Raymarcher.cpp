#include <iostream>
#include <fstream>
#include <math.h>
#include <time.h>
#include <chrono>
#include <filesystem>
#include <queue>
#include <thread>
#include "raymarcher.h"
#include "toojpeg.h"
using namespace std;
using namespace chrono_literals;

#define MAX_STEPS 100
#define MAX_DIST 1000

vec3 light = vec3(25.0, 73.0, -50.0);
vec2 resolution = vec2(1920, 1080);

clock_t start;

int numberOfUsedCores = thread::hardware_concurrency() - 1;

int frames = 120;
int framesPerSec = 30;
int finishedFrames = 0;

int stamps = 0;
double totalTime = 0.0;
double deltaTime = (1.0/(double)framesPerSec);
bool fileOpen = false;

string folder = "tmp/Frame";
unsigned char* image[120];

queue<int> saveQueue;

ofstream file;

void output(unsigned char c)
{
	file << c;
}

void renderFrame(int threadId)
{
	system("clear");
	string folder = "tmp/Frame";
	Sphere s(vec3(0.0, 0.0, 0.0), 1.0);
	vec3 cam = vec3(0.0, 0.0, 5.0);

	for(int frame = threadId; frame < frames; frame+= numberOfUsedCores)
	{
		
		image[frame] = (unsigned char*)malloc(resolution.x * resolution.y * 3 * sizeof(unsigned char));
		
		s.position.x = sin(deltaTime * frame);
		s.position.z = cos(deltaTime * frame);

		for(int y = 0; y < resolution.y; y ++)
		{
			for(int x = 0; x < resolution.x; x++)
			{
				vec3 dir = CalculateRay(90.0, vec2(resolution.x, resolution.y), vec2(x, y));
				float traveledDist = 0.0f;

				for(int i = 0; i < MAX_STEPS; i++)
				{
					vec3 currentPosition = vec3(cam.x + traveledDist * dir.x, cam.y + traveledDist * dir.y, cam.z + traveledDist * dir.z);

					float dist = s.Sdf(currentPosition);

					if(dist <= 0.001f)
					{
						int offset = (y*resolution.x+x)*3;

						vec3 normal = s.calculate_normal(currentPosition);
						vec3 dirToLight = normalize(vec3(currentPosition.x - light.x, currentPosition.y - light.y, currentPosition.z - light.z));
						float intensity = max(0.0f, dot(normal, dirToLight));
						
						image[frame][offset + 0] = 255.0 * intensity;
						image[frame][offset + 1] = 0.0;
						image[frame][offset + 2] = 0.0;
						break;

					}

					if(dist > MAX_DIST)
					{
						break;
					}

					traveledDist += dist;

				}

			}
		}

		finishedFrames++;
		saveQueue.push(frame);

	}


}

void SaveFrame(int frameId)
{
	string folder = "tmp/Frame";
	
	folder += to_string(frameId) + ".jpg";
			
	file = ofstream(folder.c_str(), ios::out | ios::binary);
	
	const bool isRGB      = true;
	const auto quality    = 90;
	const bool downsample = false;
	const char* comment = "What's up?";
	bool ok = TooJpeg::writeJpeg(output, image[frameId], resolution.x, resolution.y, isRGB, quality, downsample, comment);

	file.close();

	folder = "tmp/Frame";

	if(!ok)
	{
		cout << "Failed to save frame " << frameId << ".\n";
		getchar();
		exit(-1);
	}
	
	saveQueue.pop();
	
	
}

void Update()
{
	float progress = 0.0f;
	int lastSavedFrame = 0;
	
	do
	{
		if(progress != floor((float)finishedFrames/(float)frames * 1000.0f) / 10.0f)
		{
			progress = floor((float)finishedFrames/(float)frames * 1000.0f) / 10.0f;
			system("clear");
			cout << "Working(" << progress << "%)...\n";
		}
		
		if(!saveQueue.empty())
			SaveFrame(saveQueue.front());
		
	}while(finishedFrames < frames);
}

void render()
{
	filesystem::create_directory("tmp/");
	vector<thread> threads;

	start = clock();

	for(int i = 0; i < numberOfUsedCores; i++)
		threads.push_back(thread(renderFrame, i));

	thread updateThread(Update);

	for(int i = 0; i < numberOfUsedCores; i++)
		threads[i].join();
	
	cout << "Finished rendering all Frames. Waiting to save all...\n";
	
	updateThread.join();
	
	double time = (double)(clock()-start) / CLOCKS_PER_SEC;

	cout << "Time taken: " << time << "\n";

	cout << "Finished image render! Press any key to start merging into video...\n";

	getchar();
	system("clear");

	string cmd = "ffmpeg -r " +to_string(framesPerSec)+ " -s "+to_string((int)resolution.x)+"x"+to_string((int)resolution.y)+" -i tmp/Frame%d.jpg -vcodec libx264 -crf 0  -pix_fmt yuv420p test.mp4";

	system(cmd.c_str());
	system("clear");

	filesystem::remove_all("tmp/");
}

int main()
{
	render();

	return 0;
}
