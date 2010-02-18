// Item dump

#include <iostream>
#include <iomanip>
#include <sstream>
#include <climits>
#include <integers.h>
#include <vector>
using namespace std;

#include <DFTypes.h>
#include <DFHackAPI.h>

struct matGlosses 
{
    vector<DFHack::t_matgloss> plantMat;
    vector<DFHack::t_matgloss> woodMat;
    vector<DFHack::t_matgloss> stoneMat;
    vector<DFHack::t_matgloss> metalMat;
    vector<DFHack::t_matgloss> creatureMat;
};

string getMaterialType(DFHack::t_item item, const vector<string> & buildingTypes,const matGlosses & mat){
    if(item.type == 85 || item.type == 113 || item.type == 117) // item_plant or item_thread or item_seeds
    {
        return(string(mat.plantMat[item.material.type].id));
    }
    else if(item.type == 109 || item.type == 114 || item.type == 115 || item.type == 116 || item.type==128 || item.type == 129|| item.type == 130|| item.type == 131) // item_skin_raw item_bones item_skill item_fish_raw item_pet item_skin_tanned item_shell
    {
        return(string(mat.creatureMat[item.material.type].id));
    }
    else if(item.type == 124){ //wood
        return(string(mat.woodMat[item.material.type].id));
    }
    else if(item.type == 118){ //blocks
        return(string(mat.metalMat[item.material.index].id));
    }
    else if(item.type == 86){ // item_glob I don't know what those are in game, just ignore them
        return(string(""));
    }
    else{
        switch (item.material.type)
        {
        case 0:
            return(string(mat.woodMat[item.material.index].id));
            break;
        case 1:
            return(string(mat.stoneMat[item.material.index].id));
            break;
        case 2:
            return(string(mat.metalMat[item.material.index].id));
            break;
        case 12: // don't ask me why this has such a large jump, maybe this is not actually the matType for plants, but they all have this set to 12
            return(string(mat.plantMat[item.material.index].id));
            break;
        case 3:
        case 9:
        case 10:
        case 11:
        case 121:
            return(string(mat.creatureMat[item.material.index].id));
            break;
        default:
            //DF.setCursorCoords(item.x,item.y,item.z);
            return(string(""));
        }
    }   
}
void printItem(DFHack::t_item item, const vector<string> & buildingTypes,const matGlosses & mat){
    cout << dec << "Item at x:" << item.x << " y:" << item.y << " z:" << item.z << endl;
    cout << "Type: " << (int) item.type << " " << buildingTypes[item.type] << " Address: " << hex << item.origin << endl;
    cout << "Material: ";

    string itemType = getMaterialType(item,buildingTypes,mat);
    cout << itemType << endl;
}
int main ()
{

    DFHack::API DF ("Memory.xml");

    if(!DF.Attach())
    {
        cerr << "DF not found" << endl;
        return 1;
    }
    DF.Suspend();
    DF.InitViewAndCursor();
    matGlosses mat;
    DF.ReadPlantMatgloss(mat.plantMat);
    DF.ReadWoodMatgloss(mat.woodMat);
    DF.ReadStoneMatgloss(mat.stoneMat);
    DF.ReadMetalMatgloss(mat.metalMat);
    DF.ReadCreatureMatgloss(mat.creatureMat);

    vector <string> objecttypes;
    DF.getClassIDMapping(objecttypes);
    
    uint32_t numItems;
    DF.InitReadItems(numItems);
    DF.InitViewAndCursor();
    
    cout << "q to quit, anything else to look up items at that location\n";
    while(1)
    {
        string input;
        DF.ForceResume();
        getline (cin, input);
        DF.Suspend();
        //FIXME: why twice?
        uint32_t numItems;
        DF.InitReadItems(numItems);
        if(input == "q")
        {
            break;
        }
        
        int32_t x,y,z;
        DF.getCursorCoords(x,y,z);
        vector <DFHack::t_item> foundItems;
        for(uint32_t i = 0; i < numItems; i++)
        {
            DFHack::t_item temp;
            DF.ReadItem(i, temp);
            if(temp.x == x && temp.y == y && temp.z == z)
            {
                foundItems.push_back(temp);
                //cout << buildingtypes[temp.type] << " 0x" << hex << temp.origin << endl;
            }
        }
        if(foundItems.size() == 0){
            cerr << "No Items at x:" << x << " y:" << y << " z:" << z << endl;
        }
        else if(foundItems.size() == 1)
        {
            printItem(foundItems[0], objecttypes ,mat);
        }
        else
        {
            cerr << "Please Select which item you want to display\n";
            for(uint32_t j = 0; j < foundItems.size(); ++j)
            {
                cerr << j << " " << objecttypes[foundItems[j].type] << endl;
            }
            uint32_t value;
            string input2;
            stringstream ss;
            getline(cin, input2);
            ss.str(input2);
            ss >> value;
            while(value >= foundItems.size())
            {
                cerr << "invalid choice, please try again" << endl;
                input2.clear();
                ss.clear();
                getline (cin, input2);
                ss.str(input2);
                ss >> value;
            }
            printItem(foundItems[value], objecttypes ,mat);
        }
        DF.FinishReadItems();
    }    
    DF.FinishReadBuildings();
    DF.Detach();
#ifndef LINUX_BUILD
    cout << "Done. Press any key to continue" << endl;
    cin.ignore();
#endif
    return 0;
}