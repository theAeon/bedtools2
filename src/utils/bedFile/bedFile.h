/*****************************************************************************
  bedFile.h

  (c) 2009 - Aaron Quinlan
  Hall Laboratory
  Department of Biochemistry and Molecular Genetics
  University of Virginia
  aaronquinlan@gmail.com

  Licensed under the GNU General Public License 2.0+ license.
******************************************************************************/
#ifndef BEDFILE_H
#define BEDFILE_H

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <limits.h>
#include <cstdio>
//#include <tr1/unordered_map>
using namespace std;

//*************************************************
// Genome binning constants
//*************************************************
const int binOffsetsExtended[] =
	{4096+512+64+8+1, 512+64+8+1, 64+8+1, 8+1, 1, 0};

	
const int _binFirstShift = 17;	    /* How much to shift to get to finest bin. */
const int _binNextShift  = 3;		/* How much to shift to get to next larger bin. */


//*************************************************
// Common data structures
//*************************************************

struct DEPTH {
	unsigned int starts;
	unsigned int ends;
};

/*
	Structure for regular BED3-BED6 records
*/
struct BED {

	// UCSC BED fields
	string chrom;
	int start;
	int end; 
	string name;
	string score;
	string strand;
	
	vector<string> otherFields;

	// Additional fields
	unsigned int count;			// count of number of intervals
								// that overlap this feature
								
	map<unsigned int, DEPTH> depthMap;
	int minOverlapStart;
};


// return the genome "bin" for a feature with this start and end
int getBin(int start, int end);

	
// return the amount of overlap between two features.  Negative if none and the the 
// number of negative bases is the distance between the two.
inline 
int overlaps(int aS, int aE, int bS, int bE) {
	return min(aE, bE) - max(aS, bS);
}

// return the lesser of two values.
inline 
int min(int a, int b) {
	if (a <= b) return a;
	else return b;
}

// return the greater of two values.
inline 
int max(int a, int b) {
	if (a >= b) return a;
	else return b;
}


// templated function to convert objects to strings
template <typename T>
std::string ToString(const T & value)
{
	std::stringstream ss;
	ss << value;
	return ss.str();
}


// BED Sorting Methods 
bool sortByChrom(const BED &a, const BED &b);	
bool sortByStart(const BED &a, const BED &b);
bool sortBySizeAsc(const BED &a, const BED &b);
bool sortBySizeDesc(const BED &a, const BED &b);
bool sortByScoreAsc(const BED &a, const BED &b);
bool sortByScoreDesc(const BED &a, const BED &b);
bool byChromThenStart(BED const &a, BED const &b);



//*************************************************
// Common typedefs
//*************************************************
typedef vector<BED> bedVector;

typedef map<int, vector<BED>, std::less<int> > binsToBeds;
//typedef tr1::unordered_map<int, vector<BED> > binsToBeds;

typedef map<string, binsToBeds, std::less<string> > masterBedMap;
//typedef tr1::unordered_map<string, binsToBeds> masterBedMap;

//typedef map<string, bedVector, std::less<string> > masterBedMap;

typedef map<string, vector<BED>, std::less<string> > masterBedMapNoBin;


//************************************************
// BedFile Class methods and elements
//************************************************
class BedFile {

public:

	// Constructor 
	BedFile(string &);

	// Destructor
	~BedFile(void);
	
	void Open(void);
	
	void Close(void);
	
	bool GetNextBed (BED &bed, int &lineNum);

	// load a BED file into a map keyed by chrom, then bin. value is vector of BEDs
	void loadBedFileIntoMap();

	// load a BED file into a map keyed by chrom. value is vector of BEDs
	void loadBedFileIntoMapNoBin();	

	// Given a chrom, start, end and strand for a single feature,
	// search for all overlapping features in another BED file.
	// Searches through each relevant genome bin on the same chromosome
	// as the single feature. Note: Adapted from kent source "binKeeperFind"
	void FindOverlapsPerBin(string chrom, int start, int end, string strand, vector<BED> &hits, bool forceStrand);

	// return true if at least one overlap was found.  otherwise, return false.
	bool FindOneOrMoreOverlapsPerBin(string chrom, int start, int end, string strand, 
										bool forceStrand, float overlapFraction);

	// return true if at least one __reciprocal__ overlap was found.  otherwise, return false.
	bool FindOneOrMoreReciprocalOverlapsPerBin(string chrom, int start, int end, string strand, 
													bool forceStrand, float overlapFraction);
	

	// Given a chrom, start, end and strand for a single feature,
	// increment a the number of hits for each feature in B file
	// that the feature overlaps
	void countHits(const BED &a, bool forceStrand);
	
	// printing methods
	void reportBedTab(const BED &);
	void reportBedNewLine(const BED &);		
	void reportBedRangeTab(const BED &bed, int start, int end);
	void reportBedRangeNewLine(const BED &bed, int start, int end);
	void reportNullBedTab(void);
	void reportNullBedNewLine(void);
	
	// parse an input line and determine how it should be handled
	bool parseLine (BED &bed, const vector<string> &lineVector, int &lineNum);
		

	// the bedfile with which this instance is associated
	string bedFile;
	unsigned int bedType;  // 3-6, 12 for BED
						   // 9 for GFF
						
	bool isGff;
	masterBedMap bedMap;
	masterBedMapNoBin bedMapNoBin;
	
private:
	
	// process as line from a BED file
	bool parseBedLine (BED &bed, const vector<string> &lineVector, int lineNum);
	
	// process as line from a GFF file. convert to a BED feature.
	bool parseGffLine (BED &bed, const vector<string> &lineVector, int lineNum);
	
	void setGff (bool isGff);
};

#endif /* BEDFILE_H */
