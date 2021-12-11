#pragma once

#include <cstdint>
#include <string>

//! Default config file name when none is defined.
const std::string cDefaultFilename{"config.ini"};

struct Parameters {
    unsigned population{};                          // >= 0
    unsigned stepsPerGeneration{};                  // > 0
    unsigned maxGenerations{};                      // >= 0
    unsigned numThreads{};                          // > 0
    unsigned signalLayers{};                        // >= 0
    unsigned genomeMaxLength{1};                    // > 0
    unsigned maxNumberNeurons{1};                   // > 0
    double pointMutationRate{};                     // 0.0..1.0
    double geneInsertionDeletionRate{};             // 0.0..1.0
    double deletionRatio{};                         // 0.0..1.0
    bool killEnable{};    
    bool sexualReproduction{};    
    bool chooseParentsByFitness{};    
    float populationSensorRadius{1};                // > 0.0
    unsigned signalSensorRadius{1};                 // > 0
    float responsiveness{};                         // >= 0.0
    unsigned responsivenessCurveKFactor{1};         // 1, 2, 3, or 4
    unsigned longProbeDistance{1};                  // > 0
    unsigned shortProbeBarrierDistance{1};          // > 0
    float valenceSaturationMag{};   
    bool saveVideo{};   
    unsigned videoStride{1};                        // > 0
    unsigned videoSaveFirstFrames{};                // >= 0, overrides videoStride
    unsigned displayScale{};    
    unsigned agentSize{};   
    unsigned genomeAnalysisStride{1};               // > 0
    unsigned displaySampleGenomes{};                // >= 0
    unsigned genomeComparisonMethod{};              // 0 = Jaro-Winkler; 1 = Hamming
    bool updateGraphLog{};    
    unsigned updateGraphLogStride{1};               // > 0
    unsigned challenge{};   
    unsigned barrierType{};                         // >= 0
    unsigned replaceBarrierType{};                  // >= 0
    unsigned replaceBarrierTypeGenerationNumber{};  // >= 0

    // These must not change after initialization
    uint16_t sizeX{2};                              // 2..0x10000
    uint16_t sizeY{2};                              // 2..0x10000
    unsigned genomeInitialLengthMin{1};             // > 0 and < genomeInitialLengthMax
    unsigned genomeInitialLengthMax{1};             // > 0 and < genomeInitialLengthMin
    std::string logDir{};
    std::string imageDir{};
    std::string graphLogUpdateCommand{};
};

/*! \class ParameterIO
    \brief Responsible for setting and getting parameters
*/
class ParameterIO {
public:
    //! Returns the immutable reference of the parameters object.
    const Parameters& GetParamRef() const { return privParams; }
    //! Sets the default value for the parameter object.
    void SetDefaults();
    //! Updates parameter object from file.
    void ReadFromConfigFile(const std::string& filename);
    //! Writes the content of the parameter object in file.
    void WriteToConfigFile(const std::string& filename);
private:
    //! Parses parameter with type safety.
    void ingestParameter(std::string name, std::string val);

    Parameters privParams;
    time_t lastModTime; // when config file was last read
    
};