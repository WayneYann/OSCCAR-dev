/*****************************************************************************
*
* Copyright (c) 2000 - 2010, Lawrence Livermore National Security, LLC
* Produced at the Lawrence Livermore National Laboratory
* LLNL-CODE-400124
* All rights reserved.
*
* This file is  part of VisIt. For  details, see https://visit.llnl.gov/.  The
* full copyright notice is contained in the file COPYRIGHT located at the root
* of the VisIt distribution or at http://www.llnl.gov/visit/copyright.html.
*
* Redistribution  and  use  in  source  and  binary  forms,  with  or  without
* modification, are permitted provided that the following conditions are met:
*
*  - Redistributions of  source code must  retain the above  copyright notice,
*    this list of conditions and the disclaimer below.
*  - Redistributions in binary form must reproduce the above copyright notice,
*    this  list of  conditions  and  the  disclaimer (as noted below)  in  the
*    documentation and/or other materials provided with the distribution.
*  - Neither the name of  the LLNS/LLNL nor the names of  its contributors may
*    be used to endorse or promote products derived from this software without
*    specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR  IMPLIED WARRANTIES, INCLUDING,  BUT NOT  LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND  FITNESS FOR A PARTICULAR  PURPOSE
* ARE  DISCLAIMED. IN  NO EVENT  SHALL LAWRENCE  LIVERMORE NATIONAL  SECURITY,
* LLC, THE  U.S.  DEPARTMENT OF  ENERGY  OR  CONTRIBUTORS BE  LIABLE  FOR  ANY
* DIRECT,  INDIRECT,   INCIDENTAL,   SPECIAL,   EXEMPLARY,  OR   CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT  LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR
* SERVICES; LOSS OF  USE, DATA, OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER
* CAUSED  AND  ON  ANY  THEORY  OF  LIABILITY,  WHETHER  IN  CONTRACT,  STRICT
* LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY  WAY
* OUT OF THE  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
*
*****************************************************************************/

// ************************************************************************* //
//                             avtNamedSelection.C                           //
// ************************************************************************* //

#include <avtNamedSelection.h>

#include <iostream>
#include <iomanip>

using namespace std;

#include <avtIdentifierSelection.h>

#include <visitstream.h>
#include <VisItException.h>
#include <InvalidVariableException.h>


// ****************************************************************************
//  Method: avtNamedSelection constructor
//
//  Programmer: Hank Childs
//  Creation:   February 2, 2009
//
// ****************************************************************************

avtNamedSelection::avtNamedSelection(const std::string &n)
{
    name = n;
}


// ****************************************************************************
//  Method: avtNamedSelection destructor
//
//  Programmer: Hank Childs
//  Creation:   February 2, 2009
//
// ****************************************************************************

avtNamedSelection::~avtNamedSelection()
{
}


// ****************************************************************************
//  Method: avtZoneIdNamedSelection constructor
//
//  Programmer: Hank Childs
//  Creation:   February 2, 2009
//
// ****************************************************************************

avtZoneIdNamedSelection::avtZoneIdNamedSelection(const std::string &n)
    : avtNamedSelection(n)
{
}


// ****************************************************************************
//  Method: avtZoneIdNamedSelection constructor
//
//  Programmer: Hank Childs
//  Creation:   February 3, 2009
//
// ****************************************************************************

avtZoneIdNamedSelection::avtZoneIdNamedSelection(const std::string &n, int num,
                                                 const int *dom, const int *zon)
    : avtNamedSelection(n)
{
    domId.resize(num);
    zoneId.resize(num);
    for (int i = 0 ; i < num ; i++)
    {
        domId[i]  = dom[i];
        zoneId[i] = zon[i];
    }
}


// ****************************************************************************
//  Method: avtZoneIdNamedSelection destructor
//
//  Programmer: Hank Childs
//  Creation:   February 2, 2009
//
// ****************************************************************************

avtZoneIdNamedSelection::~avtZoneIdNamedSelection()
{
}


// ****************************************************************************
//  Method: avtZoneIdNamedSelection::Write
//
//  Programmer: Hank Childs
//  Creation:   February 2, 2009
//
//  Modifications:
//    Kathleen Bonnell, Thur Mar 26 08:16:23 MST 2009
//    Use size_t for loop variable to remove signed/unsigned int comparison
//    warnings.
//
// ****************************************************************************

void
avtZoneIdNamedSelection::Write(const std::string &fname)
{
    ofstream ofile(fname.c_str());
    ofile << ZONE_ID << endl;
    ofile << domId.size() << endl;
    for (size_t i = 0 ; i < domId.size() ; i++)
    {
        ofile << domId[i] << " " << zoneId[i] << endl;
    }
}

// ****************************************************************************
//  Method: avtZoneIdNamedSelection::Read
//
//  Programmer: Hank Childs
//  Creation:   February 2, 2009
//
// ****************************************************************************

void
avtZoneIdNamedSelection::Read(const std::string &fname)
{
    ifstream ifile(fname.c_str());
    if (ifile.fail())
    {
        EXCEPTION1(VisItException, "Cannot read named selection");
    }

    int nsType;
    ifile >> nsType;
    if (nsType != ZONE_ID)
    {
        EXCEPTION1(VisItException, "Internal error reading named selection");
    }

    int nvals;
    ifile >> nvals;
    if (nvals < 0)
    {
        EXCEPTION1(VisItException, "Invalid named selection");
    }
  
    domId.resize(nvals);
    zoneId.resize(nvals);
    for (int i = 0 ; i < nvals ; i++)
    {
        ifile >> domId[i];
        ifile >> zoneId[i];
    }
}


// ****************************************************************************
//  Method: avtZoneIdNamedSelection::GetDomainList
//
//  Purpose:
//      Gets the list of domains from the identifiers in this selection.
//
//  Programmer: Hank Childs
//  Creation:   February 3, 2009
//
//  Modifications:
//    Kathleen Bonnell, Thur Mar 26 08:16:23 MST 2009
//    Use size_t for loop variable to remove signed/unsigned int comparison
//    warnings.
//
// ****************************************************************************

bool
avtZoneIdNamedSelection::GetDomainList(std::vector<int> &domains)
{
    int maxDomain = 0;
    for (size_t i = 0 ; i < domId.size() ; i++)
    {
        if (domId[i] > maxDomain)
            maxDomain = domId[i];
    }

    std::vector<bool> useDomain(maxDomain+1, false);
    int numDoms = 0;
    for (size_t i = 0 ; i < domId.size() ; i++)
    {
        if (! useDomain[domId[i]])
        {
            useDomain[domId[i]] = true;
            numDoms++;
        }
    }

    domains.resize(numDoms);
    int curDom = 0;
    for (int i = 0 ; i < maxDomain+1 ; i++)
        if (useDomain[i])
            domains[curDom++] = i;

    return true;
}


// ****************************************************************************
//  Method: avtZoneIdNamedSelection::GetMatchingIds
//
//  Purpose:
//      Get the zone indices that match the named selection.
//
//  Programmer: Hank Childs
//  Creation:   February 3, 2009
//
//  Modifications:
//    Tom Fogal, Sat Feb  7 17:13:46 EST 2009
//    Fix declaration of iterator, and make it const.
//
//    Kathleen Bonnell, Thur Mar 26 08:16:23 MST 2009
//    Use size_t for loop variable to remove signed/unsigned int comparison
//    warnings.  Added PairCompare to iterator specification for compilation
//    on windows.
//
// ****************************************************************************

void
avtZoneIdNamedSelection::GetMatchingIds(unsigned int *pts, int nvals, 
                                        std::vector<int> &ids)
{
    if (lookupSet.size() == 0)
    {
        for (size_t i = 0 ; i < domId.size() ; i++)
        {
            IntPair ip;
            ip.d = domId[i];
            ip.z = zoneId[i];
            lookupSet.insert(ip);
        }
    }

    for (int i = 0 ; i < nvals ; i++)
    {
        IntPair ip;
        ip.d = (int) pts[2*i];
        ip.z = (int) pts[2*i+1];
        std::set<IntPair, PairCompare>::const_iterator it;
        it = lookupSet.find(ip);
        if (it != lookupSet.end())
            ids.push_back(i);
    }
}


// ****************************************************************************
//  Method: avtFloatingPointIdNamedSelection constructor
//
//  Programmer: Hank Childs
//  Creation:   February 2, 2009
//
// ****************************************************************************

avtFloatingPointIdNamedSelection::avtFloatingPointIdNamedSelection(
                                                          const std::string &n)
    : avtNamedSelection(n)
{
}


// ****************************************************************************
//  Method: avtFloatingPointIdNamedSelection constructor
//
//  Programmer: Hank Childs
//  Creation:   February 23, 2009
//
// ****************************************************************************

avtFloatingPointIdNamedSelection::avtFloatingPointIdNamedSelection(
                                                const std::string &n,
                                                const std::vector<double> &ids2)
    : avtNamedSelection(n)
{
    ids = ids2;
}


// ****************************************************************************
//  Method: avtFloatingPointIdNamedSelection destructor
//
//  Programmer: Hank Childs
//  Creation:   February 2, 2009
//
// ****************************************************************************

avtFloatingPointIdNamedSelection::~avtFloatingPointIdNamedSelection()
{
}


// ****************************************************************************
//  Method: avtFloatingPointIdNamedSelection::Write
//
//  Programmer: Hank Childs
//  Creation:   February 2, 2009
//
//  Modifications:
//    Kathleen Bonnell, Thur Mar 26 08:16:23 MST 2009
//    Use size_t for loop variable to remove signed/unsigned int comparison
//    warnings.
//
//    Gunther H. Weber, Mon Apr  6 20:17:40 PDT 2009
//    Increased precision for floating point output.
//
// ****************************************************************************

void
avtFloatingPointIdNamedSelection::Write(const std::string &fname)
{
    ofstream ofile(fname.c_str());
    ofile << FLOAT_ID << endl;
    ofile << ids.size() << endl;
    ofile << std::setprecision(32);
    for (size_t i = 0 ; i < ids.size() ; i++)
    {
        ofile << ids[i] << endl;
    }
}

// ****************************************************************************
//  Method: avtFloatingPointIdNamedSelection::Read
//
//  Programmer: Hank Childs
//  Creation:   February 2, 2009
//
// ****************************************************************************

void
avtFloatingPointIdNamedSelection::Read(const std::string &fname)
{
    ifstream ifile(fname.c_str());
    if (ifile.fail())
    {
        EXCEPTION1(VisItException, "Cannot read named selection");
    }

    int nsType;
    ifile >> nsType;
    if (nsType != FLOAT_ID)
    {
        EXCEPTION1(VisItException, "Internal error reading named selection");
    }

    int nvals;
    ifile >> nvals;
    if (nvals < 0)
    {
        EXCEPTION1(VisItException, "Invalid named selection");
    }
  
    ids.resize(nvals);
    for (int i = 0 ; i < nvals ; i++)
    {
        ifile >> ids[i];
    }
}


// ****************************************************************************
//  Method: avtFloatingPointIdNamedSelection::CreateSelection
//
//  Purpose:
//      Creates an avtDataSelection for the database to read in only the data
//      corresponding to this selection.
//
//  Programmer: Hank Childs
//  Creation:   February 23, 2009
//
// ****************************************************************************

avtDataSelection *
avtFloatingPointIdNamedSelection::CreateSelection(void)
{
    avtIdentifierSelection *rv = new avtIdentifierSelection;
    rv->SetIdentifiers(ids);
    return rv;
}


// ****************************************************************************
//  Method: avtFloatingPointIdNamedSelection::CreateConditionString
//
//  Purpose:
//      Creates a condition string that can be fed to the database.
//
//  Notes:      This is probably a bad design, because it assumes FastBit.
//              This should be revisited soon.
//
//  Programmer: Hank Childs
//  Creation:   April 6, 2009
//
// ****************************************************************************

static std::string
stringify(double x)
{
    std::ostringstream o;
    if (!(o << setprecision(32) << x))
    {
        EXCEPTION1(InvalidVariableException, "string conversion");
    }
    return o.str();
}

const std::string
avtFloatingPointIdNamedSelection::CreateConditionString(void)
{
    std::string condition = "";
    if (ids.size()>0)
    {
        // convert all the identifiers into a string...
        std::string id_string = "( id in ( "; // Prabhat- TODO id is hardcoded here..

        for (int j=0; j<ids.size()-1; j++)
            id_string = id_string + stringify(ids[j]) + ", ";
        id_string = id_string + stringify(ids[ids.size()-1]) + " ))";

        condition = id_string;
    }
    return condition;
}


