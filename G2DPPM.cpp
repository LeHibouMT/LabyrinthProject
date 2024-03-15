////////////////////////////////////////////////////////
//
//      	load ppm image file into texture
//
///////////////////////////////////////////////////////


#include <string>
#include <memory>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;
#include "G2D.h"

using byte = unsigned char;

// create texture from RGBA data (G2D.cpp)
int CORECreateTextureFromRGBA(unsigned char * data, int width, int height);

// skip line starting with    # blablabla
void SkipComments(ifstream & infile)
{
	char c;
	infile.get(c);  // read one character
	if (c == '#')   // detecting comments
	{
		while (c != '\n')  {	infile.get(c);   }   // skip whole line
	}
	else
	{
		infile.seekg(-1, infile.cur);  // not a #, going back
	}
}

// lazy error management
void GeneralError(string msg)
{
	cout << msg << endl;
	cout.flush();
	char c;
	cin >> c; // wait
}

// convert RGB data to RGBA data
void ExtendRGBtoRGBA(vector<byte> & data)
{
	int nbPixels = data.size();
	data.resize(nbPixels * 4); // RGBA

	// shift RGB color to create RGBA colors 
	// starting from the end of the array

	int pRGBA = nbPixels * 4 - 4;
	int pRGB = nbPixels * 3 - 3;

	for (int i = 0; i < nbPixels; i++)
	{
		data[pRGBA + 3] = 255;
		data[pRGBA + 2] = data[pRGB + 2];
		data[pRGBA + 1] = data[pRGB + 1];
		data[pRGBA + 0] = data[pRGB + 0];
		pRGB -= 3;
		pRGBA -= 4;
	}
}

 
// V-symetry

void VsymetryRGBAImage(vector<byte> & data,int Width, int Height)
{
	for (int y = 0; y < Width / 2; y++)
	{
		int width = Width * 4;
		int pY = y * width;
		int sY = (Height - y - 1) * width;

		for (int x = 0; x < width; x++)
		{
			unsigned char t = data[pY + x];
			data[pY + x] = data[sY + x];
			data[sY + x] = t;
		}
	}
}

// any pixel with given RGB color value becomes transparent
void SetTransparentPixels(vector<byte> & data, byte R, byte G, byte B)
{
	int nbPixels = data.size() / 4;
	
	int p = 0;
	for (int i = 0; i < nbPixels; i++)
	{
		if (data[p] == R && data[p + 1] == G && data[p + 2] == B)
			data[p + 3] = 0;
		p += 4;
	}
}

// path filename should be relative : "toto.ppm"
// applyTransparency => color of the top left pixel is used as a colorkey for transparency

int G2D::ExtractTextureFromPPM(const std::string & filepath, bool applyTransparency)
{
	string mMagic;
	int Height, Width, maxBits;

	ifstream infile(filepath, ifstream::binary);
	 
	if ( ! infile.is_open() )  GeneralError("Failed to open : " + filepath);

	SkipComments(infile);

	infile >> mMagic;

	SkipComments(infile);

	infile >> Width >> Height;

	SkipComments(infile); 

	infile >> maxBits;

	if (maxBits != 255)  GeneralError("Non 8 bits format");

	vector<byte> data;  // used to store color values

	int nbPixels = Width * Height;
	 
	if (mMagic == "P3")   // ASCII data
	{
		string R, G, B;
		data.resize(nbPixels * 4); // RGBA

		for (int i = 0; i < nbPixels ; i++)
		{
			infile >> R >> G >> B;
			data.push_back(stoi(R));
			data.push_back(stoi(G));
			data.push_back(stoi(B));
			data.push_back(255);
		}
	}
	else if (mMagic == "P6")  // Binary data
	{
		infile.seekg(1, infile.cur); // Move curser once to skip '\n'
		data.resize(nbPixels * 3); // RGB format
		infile.read((char*)data.data(), nbPixels * 3);  // read all binary data 
		ExtendRGBtoRGBA(data);
	}
	else
		GeneralError("Unrecognized PPM format");

	if (applyTransparency)  // all pixels whose color is identical to the first pixel become transparent
	{
		byte r = data[0];  // select color of top left pixels
		byte g = data[1];
		byte b = data[2];
		
		SetTransparentPixels(data, r, g, b);
	}

	VsymetryRGBAImage(data, Width, Height);
	

	int id = CORECreateTextureFromRGBA(data.data(), Width, Height);
	return id;
	
}