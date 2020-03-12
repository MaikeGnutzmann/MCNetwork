#include "parameterstorage.h"
ParameterStorage::ParameterStorage(std::string filename)
{
    DEBUG_FUNC_START

    std::ifstream infile(filename);
    std::string line;
    double val;  //buffer for values
    std::string name; //buffer for strings

    while (std::getline(infile, line))
    {   
        // std::cout<<line<<std::endl;
        //remove comments
        auto pos=line.find("#");
        if (pos==0 or line.size()==0) continue;
        if (pos==std::string::npos);
        else
        {
            line.erase(pos,std::string::npos);
        }

        std::istringstream iss(line);
        if(!(iss>>name)) throw std::invalid_argument( "cant read line: " + line);
        if(name == "electrode"){
            double pos, voltage;
            int edge;
            if (geometry == "rect"){
                if(!(iss>>pos>>edge>>voltage)) throw std::invalid_argument( "cant read electrode: " + line);
                electrodes.push_back({pos,edge,voltage});
            }
            else if (geometry == "circle"){
                if(!(iss>>pos>>voltage)) throw std::invalid_argument( "cant read electrode: " + line);
                electrodes.push_back({pos,0,voltage});
            }
            else{
                throw std::invalid_argument( "'geometry' needs to be set first");
            }
        }
        else if (name == "gate"){
            if(!(iss>>gate)) throw std::invalid_argument( "cant read gate: " + line);
        }
        else if (name == "geometry"){
            if(!(iss>>geometry)) throw std::invalid_argument( "cant read geometry: " + line);
            if(geometry != "rect" and geometry != "circle") throw std::invalid_argument( "invalid value for geometry: '" + line + "' valid: rect, circle");
        }
        else{
            if(!(iss>>val)) throw std::invalid_argument( "can't read line: " + line);
            parameters[name]=val;    
            // std::cout<<name<<parameters[name]<<std::endl;
        }
    }


    parameters["kT"]=parameters["k"]*parameters["T"];
    parameters["hoppingSiteNumber"]=parameters["acceptorNumber"]+electrodes.size();

    // convert lens in dimensions of R    
    parameters["R"]=std::sqrt(parameters["lenX"]*parameters["lenY"]/parameters["acceptorNumber"]);
    parameters["minHoppingDist"]     = parameters["minHoppingDist"]    /parameters["R"];
    parameters["maxHoppingDist"]     = parameters["maxHoppingDist"]    /parameters["R"];
    parameters["maxInteractionDist"] = parameters["maxInteractionDist"]/parameters["R"];
    parameters["a"]                  = parameters["a"]                 /parameters["R"];
    parameters["minDist"]            = parameters["minDist"]           /parameters["R"];
    parameters["electrodeWidth"]     = parameters["electrodeWidth"]    /parameters["R"];
    if (geometry == "rect"){
        parameters["lenX"]           = parameters["lenX"]              /parameters["R"];
        parameters["lenY"]           = parameters["lenY"]              /parameters["R"];
    }
    else if (geometry == "circle"){
        parameters["radius"]         = parameters["radius"]            /parameters["R"];
    }

    std::cout<<"R = "<<parameters["R"]<<" nm, a/R = "<<parameters["a"]<<", I0 = "<<parameters["I0"]<<" meV == "<<parameters["I0"]*0.001*parameters["e"]/parameters["kT"]<<"kT corresponds to eps = "<< parameters["e"]*parameters["e"]/(4*M_PI*parameters["eps0"]*parameters["R"]*1e-9*parameters["I0"]*0.001*parameters["e"]) <<std::endl;

    // convert energies in dimensions of kT
    parameters["I0"]=parameters["I0"]*0.001*parameters["e"]/parameters["kT"]; //I0 given in meV




    if (parameters.at("voltageScanPoints") != 1){parameters["voltageScanResoultion"] = (parameters.at("voltageScanMax") - parameters.at("voltageScanMin")) / (parameters.at("voltageScanPoints")-1);}
    else{                                        parameters["voltageScanResoultion"] = 0;}
    for (size_t i = 0; i < parameters.at("voltageScanPoints"); i++){
        inputVoltages.push_back(parameters.at("voltageScanMin")+i*parameters.at("voltageScanResoultion"));
    }
    std::cout<<"Searching on "<<parameters["voltageScanPoints"] <<" Voltage Points";
    if (inputVoltages.size() < 8){
        std::cout<<":"<<std::endl;
        for (size_t i = 0; i < parameters["voltageScanPoints"]; i++){
            std::cout<<inputVoltages[i]<<" ";
        }
        std::cout<<std::endl;
    }
    else{
        std::cout<<std::endl;
    }
    


    DEBUG_FUNC_END
}
