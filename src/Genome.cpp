#include "Genome.h"

#include "Parameters.h"
#include "PeepsPool.h"
#include "SensorsActions.h"

#include <cassert>
#include <iostream>

namespace Genetics
{

//---------------------------------------------------------------------------
//! Approximate gene match: Has to match same source, sink, with similar weight
//---------------------------------------------------------------------------
bool genesMatch(const Gene &g1, const Gene &g2)
{
    return g1.sinkNum == g2.sinkNum
        && g1.sourceNum == g2.sourceNum
        && g1.sinkType == g2.sinkType
        && g1.sourceType == g2.sourceType
        && g1.weight == g2.weight;
}

//---------------------------------------------------------------------------
//! The jaro_winkler_distance() function is adapted from the C version at
//! https://github.com/miguelvps/c/blob/master/jarowinkler.c
//! under a GNU license, ver. 3. This comparison function is useful if
//! the simulator allows genomes to change length, or if genes are allowed
//! to relocate to different offsets in the genome. I.e., this function is
//! tolerant of gaps, relocations, and genomes of unequal lengths.
//---------------------------------------------------------------------------
float jaro_winkler_distance(const Genome &genome1, const Genome &genome2) {
    float dw;
    auto max = [](int a, int b) { return a > b ? a : b; };
    auto min = [](int a, int b) { return a < b ? a : b; };

    const auto &s = genome1;
    const auto &a = genome2;

    int i, j, l;
    int m = 0, t = 0;
    int sl = s.size(); // strlen(s);
    int al = a.size(); // strlen(a);

    constexpr unsigned maxNumGenesToCompare = 20;
    sl = min(maxNumGenesToCompare, sl); // optimization: approximate for long genomes
    al = min(maxNumGenesToCompare, al);

    std::vector<int> sflags(sl, 0);
    std::vector<int> aflags(al, 0);
    int range = max(0, max(sl, al) / 2 - 1);

    if (!sl || !al)
        return 0.0;

    /* calculate matching characters */
    for (i = 0; i < al; i++) {
        for (j = max(i - range, 0), l = min(i + range + 1, sl); j < l; j++) {
            if (genesMatch(a[i], s[j]) && !sflags[j]) {
                sflags[j] = 1;
                aflags[i] = 1;
                m++;
                break;
            }
        }
    }

    if (!m)
        return 0.0;

    /* calculate character transpositions */
    l = 0;
    for (i = 0; i < al; i++) {
        if (aflags[i] == 1) {
            for (j = l; j < sl; j++) {
                if (sflags[j] == 1) {
                    l = j + 1;
                    break;
                }
            }
            if (!genesMatch(a[i], s[j]))
                t++;
        }
    }
    t /= 2;

    /* Jaro distance */
    dw = (((float)m / sl) + ((float)m / al) + ((float)(m - t) / m)) / 3.0f;
    return dw;
}

//---------------------------------------------------------------------------
//! Works only for genomes of equal length
//---------------------------------------------------------------------------
float hammingDistanceBits(const Genome &genome1, const Genome &genome2)
{
    assert(genome1.size() == genome2.size());

    const char* p1 = (const char*)genome1.data();
    const char* p2 = (const char*)genome2.data();
    const unsigned numElements = genome1.size();
    const unsigned bytesPerElement = sizeof(genome1[0]);
    const unsigned lengthBytes = numElements * bytesPerElement;
    const unsigned lengthBits = lengthBytes * 8;
    unsigned bitCount = 0;

    for (unsigned index = 0; index < genome1.size(); ++p1, ++p2, ++index) {
        bitCount += __builtin_popcount(*p1 ^ *p2);
    }

    // For two completely random bit patterns, about half the bits will differ,
    // resulting in c. 50% match. We will scale that by 2X to make the range
    // from 0 to 1.0. We clip the value to 1.0 in case the two patterns are
    // negatively correlated for some reason.
    return 1.0 - std::min(1.0, (2.0 * bitCount) / (float)lengthBits);
}

//---------------------------------------------------------------------------
//! Works only for genomes of equal length
//---------------------------------------------------------------------------
float hammingDistanceBytes(const Genome &genome1, const Genome &genome2)
{
    assert(genome1.size() == genome2.size());

    const char* p1 = (const char*)genome1.data();
    const char* p2 = (const char*)genome2.data();
    const unsigned numElements = genome1.size();
    const unsigned bytesPerElement = sizeof(genome1[0]);
    const unsigned lengthBytes = numElements * bytesPerElement;
    unsigned byteCount = 0;

    for (unsigned index = 0; index < genome1.size(); ++p1, ++p2, ++index) {
        byteCount += (unsigned)(*p1 == *p2);
    }

    return byteCount / (float)lengthBytes;
}

//---------------------------------------------------------------------------
float genomeSimilarity(const Genome &g1, const Genome &g2, const Parameters& params)
{
    switch (params.genomeComparisonMethod) {
    case 0:
        return jaro_winkler_distance(g1, g2);
    case 1:
        return hammingDistanceBits(g1, g2);
    case 2:
        return hammingDistanceBytes(g1, g2);
    default:
        assert(false);
    }
}

//---------------------------------------------------------------------------
void cropLength(Genome &genome, unsigned length, RandomUintGenerator& random)
{
    if (genome.size() > length && length > 0) {
        if (random() / (float)RANDOM_UINT_MAX < 0.5) {
            // trim front
            unsigned numberElementsToTrim = genome.size() - length;
            genome.erase(genome.begin(), genome.begin() + numberElementsToTrim);
        } else {
            // trim back
            genome.erase(genome.end() - (genome.size() - length), genome.end());
        }
    }
}

//-------------------------------------------------------------------------
Genetics::Gene makeRandomGene(RandomUintGenerator& random)
{
    Genetics::Gene gene;

    gene.sourceType = random() & 1;
    gene.sourceNum = (uint16_t)random(0, 0x7fff);
    gene.sinkType = random() & 1;
    gene.sinkNum = (uint16_t)random(0, 0x7fff);
    gene.weight = Genetics::Gene::makeRandomWeight(random);

    return gene;
}

//---------------------------------------------------------------------------
void randomInsertDeletion(Genome &genome, RandomUintGenerator& random, const Parameters& params)
{
    float probability = params.geneInsertionDeletionRate;
    if (random() / (float)RANDOM_UINT_MAX < probability) {
        if (random() / (float)RANDOM_UINT_MAX < params.deletionRatio) {
            // deletion
            if (genome.size() > 1) {
                genome.erase(genome.begin() + random(0, genome.size() - 1));
            }
        } else if (genome.size() < params.genomeMaxLength) {
            // insertion
            //genome.insert(genome.begin() + randomUint(0, genome.size() - 1), makeRandomGene(random));
            genome.push_back(Genetics::makeRandomGene(random));
        }
    }
}

//---------------------------------------------------------------------------
void randomBitFlip(Genome &genome, RandomUintGenerator& random)
{
    int method = 1;

    unsigned byteIndex = random(0, genome.size() - 1) * sizeof(Gene);
    unsigned elementIndex = random(0, genome.size() - 1);
    uint8_t bitIndex8 = 1 << random(0, 7);

    if (method == 0) {
        ((uint8_t *)&genome[0])[byteIndex] ^= bitIndex8;
    } else if (method == 1) {
        float chance = random() / (float)RANDOM_UINT_MAX; // 0..1
        if (chance < 0.2) { // sourceType
            genome[elementIndex].sourceType ^= 1;
        } else if (chance < 0.4) { // sinkType
            genome[elementIndex].sinkType ^= 1;
        } else if (chance < 0.6) { // sourceNum
            genome[elementIndex].sourceNum ^= bitIndex8;
        } else if (chance < 0.8) { // sinkNum
            genome[elementIndex].sinkNum ^= bitIndex8;
        } else { // weight
            genome[elementIndex].weight ^= (1 << random(1, 15));
        }
    } else {
        assert(false);
    }
}

//---------------------------------------------------------------------------
void applyPointMutations(Genome &genome, RandomUintGenerator& random, const Parameters& params)
{
    unsigned numberOfGenes = genome.size();
    while (numberOfGenes-- > 0) {
        if ((random() / (float)RANDOM_UINT_MAX) < params.pointMutationRate) {
            randomBitFlip(genome, random);
        }
    }
}

//---------------------------------------------------------------------------
void displaySampleGenomes(unsigned count, const PeepsPool& peeps, const Parameters& params)
{
    unsigned index = 1; // indexes start at 1
    for (index = 1; count > 0 && index <= params.population; ++index) {
        if (peeps[index].alive) {
            std::cout << "---------------------------\nIndividual ID " << index << std::endl;
            peeps[index].printGenome();
            std::cout << std::endl;

            //peeps[index].printNeuralNet();
            peeps[index].printIGraphEdgeList();


            std::cout << "---------------------------" << std::endl;
            --count;
        }
    }

    SensorsActions::displaySensorActionReferenceCounts(peeps, params);
}

//---------------------------------------------------------------------------
uint8_t makeGeneticColor(const Genome &genome)
{
    return ((genome.size() & 1)
         | ((genome.front().sourceType)    << 1)
         | ((genome.back().sourceType)     << 2)
         | ((genome.front().sinkType)      << 3)
         | ((genome.back().sinkType)       << 4)
         | ((genome.front().sourceNum & 1) << 5)
         | ((genome.front().sinkNum & 1)   << 6)
         | ((genome.back().sourceNum & 1)  << 7));
}

//---------------------------------------------------------------------------
float averageGenomeLength(const PeepsPool& peeps, RandomUintGenerator& random, const Parameters& params)
{
    unsigned count = 100;
    unsigned numberSamples = 0;
    unsigned long sum = 0;

    while (count-- > 0) {
        sum += peeps[random(1, params.population)].genome.size();
        ++numberSamples;
    }
    return sum / numberSamples;
}

//---------------------------------------------------------------------------
float geneticDiversity(const PeepsPool& peeps, RandomUintGenerator& random, const Parameters& params)
{
    if (params.population < 2) {
        return 0.0;
    }

    // count limits the number of genomes sampled for performance reasons.
    unsigned count = std::min(1000U, params.population);    // todo: !!! p.analysisSampleSize;
    int numSamples = 0;
    float similaritySum = 0.0f;

    while (count > 0) {
        unsigned index0 = random(1, params.population - 1); // skip first and last elements
        unsigned index1 = index0 + 1;
        similaritySum += genomeSimilarity(peeps[index0].genome, peeps[index1].genome, params);
        --count;
        ++numSamples;
    }
    float diversity = 1.0f - (similaritySum / numSamples);
    return diversity;
}

} // namespace Genetics