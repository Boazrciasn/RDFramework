#ifndef UTILDATAMODIFIER_H
#define UTILDATAMODIFIER_H

class UtilDataModifier
{
public:
    static inline cv::Mat_<float> get_hogdescriptor_mat(std::vector<float>& descriptorValues, const cv::Size & size )
    {
        int cellSize = 8;
        int binSize = 9;

        int cells_x = size.width / cellSize;
        int cells_y = size.height / cellSize;

        cv::Mat_<float> out = cv::Mat_<float>::zeros(cells_y,cells_x*binSize);

        //  keep number of updates for each cell for cell hist normalization
        int** cellUpdateCounter   = new int*[cells_y];
        for (int y=0; y<cells_y; ++y)
        {
            cellUpdateCounter[y] = new int[cells_x];
            for (int x=0; x<cells_x; ++x)
                cellUpdateCounter[y][x] = 0;
        }


        // num of blocks = num of cells - 1
        // since there is a new block on each cell (overlapping blocks!) but the last one
        int blocks_x = cells_x - 1;
        int blocks_y = cells_y - 1;

        // compute gradient strengths per cell
        int descriptorDataIdx = 0;
        int cellx = 0;
        int celly = 0;

        for (int blockx = 0; blockx < blocks_x; ++blockx)
            for (int blocky = 0; blocky < blocks_y; ++blocky)
                // 4 cells per block ...
                for (int cellNum = 0; cellNum < 4; ++cellNum)
                {
                    cellx = blockx + cellNum / 2;
                    celly = blocky + cellNum % 2;

                    ++cellUpdateCounter[celly][cellx];
                    for (int bin = 0; bin < binSize; ++bin)
                        out(celly, cellx * binSize + bin) += descriptorValues[descriptorDataIdx++];
                }


        // compute average gradient strengths
        for (celly = 0; celly < cells_y; celly++)
            for (cellx = 0; cellx < cells_x; cellx++)
                for (int bin = 0; bin < binSize; bin++)
                    out(celly, cellx * binSize + bin) /= (float)cellUpdateCounter[celly][cellx];

        // compute dominant angle
        cv::Mat_<float> outNew = cv::Mat_<float>::zeros(cells_y,cells_x);

        for (celly = 0; celly < cells_y; celly++)
            for (cellx = 0; cellx < cells_x; cellx++)
            {
                int maxIndx = 0;
                for (int bin = 1; bin < binSize; bin++)
                    if(out(celly, cellx* binSize + bin) > out(celly, cellx* binSize + maxIndx))
                        maxIndx = bin;

                if( out(celly, cellx* binSize + maxIndx) > 0.30)
                    outNew(celly, cellx) = out(celly, cellx* binSize + maxIndx); //(maxIndx+1)*20;
            }


        // free memory allocated
        for (int y = 0; y < cells_y; y++)
            delete[] cellUpdateCounter[y];
        delete[] cellUpdateCounter;
        return outNew;
    }
};

#endif // UTILDATAMODIFIER_H
