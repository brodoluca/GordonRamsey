#include "Truck.hpp"



caf::actor spawnNewTruck(caf::actor_system& system,std::string name, std::string host , uint16_t port, uint16_t own_port){
    auto truck_actor = system.spawn(truck);
    auto server_actor = system.middleman().spawn_client(TruckClient, host, port,truck_actor);
    std::srand((unsigned)std::time(nullptr));
    auto web_server = system.middleman().spawn_server(server, own_port+1,truck_actor);
    caf::scoped_actor self{system};
    try {
        if (!server_actor) {
            throw "Are you sure the Ip and the port are correct? ";
        }
        print_on_exit(*server_actor, "CLIENT");
        send_as(*server_actor,truck_actor, initialize_atom_v, name, own_port);
        send_as(*server_actor,truck_actor, update_port_host_atom_v, own_port,host);
    } catch (const char* msg) {
        self->send_exit(truck_actor, caf::exit_reason::user_shutdown);
        self->send_exit(*web_server, caf::exit_reason::user_shutdown);
        self->send(truck_actor, initialize_atom_v, name, own_port);
        std::cerr << "failed to spawn "<< name << "'s client: " << to_string(server_actor.error()) <<"\n"<< msg << "\n\n"<< std::endl;
    }
    
    print_on_exit(truck_actor, name);
    
    return truck_actor;
}




caf::actor spawnNewMaster(caf::actor_system& system,std::string name, std::string host , uint16_t port){
    
    auto truck_actor = system.spawn(truck);
    std::srand((unsigned)std::time(NULL));
    caf::scoped_actor self{system};
    auto server_actor = system.middleman().spawn_server(temp_master_server,port, truck_actor);
    try {
        if (!server_actor) {
            throw "I can not spawn a new server.";
        }
        print_on_exit(*server_actor, "TEMP_SERVER");
        send_as(*server_actor,truck_actor, update_port_host_atom_v, port,host);
        send_as(*server_actor,truck_actor, initialize_atom_v, name);
    } catch (const char* msg) {
        self->send_exit(truck_actor, caf::exit_reason::user_shutdown);
        std::cerr << msg << ": "<< to_string(server_actor.error()) <<"\n" << std::endl;
        
    }
    auto web_server = system.middleman().spawn_server(server, port+1,truck_actor);
   
    print_on_exit(truck_actor, name);
    
    
    return truck_actor;
    
}



int spawnNewClient(caf::actor_system& system,std::string name, std::string host , uint16_t port, uint16_t own_port, const caf::actor& buddy){
    auto server_actor = system.middleman().spawn_client(TruckClient, host, port,buddy);
    caf::scoped_actor self{system};
    try {
        if (!server_actor) {
            throw "Are you sure the Ip and the port are correct? ";
        }
        print_on_exit(*server_actor, "CLIENT");
        send_as(*server_actor,buddy, initialize_atom_v, own_port);
        send_as(*server_actor,buddy, update_port_host_atom_v, own_port,host);
        
        return 0;
    } catch (const char* msg) {
        
        self->send(buddy, initialize_atom_v, name, own_port);
        std::cerr << "failed to spawn "<< name << "'s client: " << to_string(server_actor.error()) <<"\n"<< msg << "\n\n"<< std::endl;
        return -1;
    }
    
}



void setTruckInformation(std::string pathLength,
                         std::string nSensor,
                         std::string nReparation,
                         std::string fuelConsumption,
                         std::string yearOfConstruction,
                         std::string horsePower,
                         std::string mileage,const caf::actor& buddy ){
    
    caf::scoped_actor self{buddy.home_system()};
    self->send(buddy, set_truck_information_v, pathLength,nSensor,nReparation,fuelConsumption,yearOfConstruction,horsePower,mileage);
    
    Py_Initialize();
    PyObject* sysPath = PySys_GetObject((char*)"path");
    PyList_Insert(sysPath, 0,PyUnicode_FromString("."));
    // Load the module
    PyObject *pName = PyUnicode_FromString("main");
    PyObject *pModule = PyImport_Import(pName);
    if (pModule != NULL) {
        ///calls the function
        PyObject* pFunc = PyObject_GetAttrString(pModule, "canBeMaster");
        PyObject *pArgs = PyTuple_Pack(7, PyUnicode_FromString(pathLength.c_str())
                                       , PyUnicode_FromString(nSensor.c_str())
                                       , PyUnicode_FromString(nReparation.c_str())
                                       , PyUnicode_FromString(fuelConsumption.c_str())
                                       , PyUnicode_FromString(yearOfConstruction.c_str())
                                       , PyUnicode_FromString(horsePower.c_str())
                                       , PyUnicode_FromString(mileage.c_str()));
        if(pFunc != NULL){
            PyObject *pValue = PyObject_CallObject(pFunc, pArgs);
            auto result = _PyUnicode_AsString(pValue);
            if(result){
//                self->state.setPossibilityToBeMaster(result);
                self->send(buddy, set_master_probability_v, result);
            }
                
        }
    }
    Py_Finalize();
    
    
    
}

void calculateTruckProbability(const caf::actor& buddy){
    caf::scoped_actor self{buddy.home_system()};
    self->send(buddy, python_atom_v);
}
