#pragma once

#include "gdal_priv.h"
#include "cpl_conv.h"
#include "gdalwarper.h"
#include "tiffio.h"

typedef std::string String;

class Geotiff {

private:
    char* filename;        // name of Geotiff
    GDALDataset* geotiffDataset; // Geotiff GDAL datset object. 
    double geotransform[6];      // 6-element geotranform array.
    int dimensions[3];           // X,Y, and Z dimensions. 
    int NROWS, NCOLS, NLEVELS;     // dimensions of data in Geotiff. 

public:

    Geotiff();
    ~Geotiff();

    void Open(char* tiffname);
    void Close();

    char* GetFileName();
    const char* GetProjection();
    double* GetGeoTransform();
    double GetNoDataValue();
    int* GetDimensions();
    float** GetRasterBand(int z);
    template<typename T>
    float** GetArray2D(int layerIndex, float** bandLayer);
};