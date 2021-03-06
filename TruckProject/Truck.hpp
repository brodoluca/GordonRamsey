
#ifndef Truck_hpp
#define Truck_hpp

#include <stdio.h>
#include <string>
#include <iostream>


#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <stdlib.h>
#include <time.h>

#include "string.h"


#include "caf/all.hpp"
#include "caf/io/all.hpp"

#include "InputMonitor.hpp"
#include "utils.h"
#include "Python.h"

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <array>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

///  To have a better idea of how this works, check our github
///  There you can find documentation and all of these files as well
///  I tried to do my best to make this code as understandable as possible, however I have my limitations. In the github you will find some further documentation which will be usefull. I truly apologize. 
///   https://github.com/brodoluca/GordonRamsey




struct Truck{
    ///We keep those public because otherwise it would be a pain to handle them
public:
    ///stores the pointer od the server
    caf::strong_actor_ptr server;
    ///stores the pointer of the client
    caf::strong_actor_ptr client;
    ///keeps truck of the platoon size
    truck_quantity tqPlatoon = 1;
    
    ///Thos should be inside the union, but for some reasons, if I do, the code wont compile
    ///im pretty sure this is due to the fact that I initialize them here
    
    ///if the truck is the one initializing the update of the platoon size
    bool initiate_update = false;
    ///0 for no switcheroo (basic state)
    ///1 for started switcheroo
    ///2 for switchereee(the guy who gets switched)
    int initiated_switcheroo = 0;
    
    bool initiate_port_host_update;
    
    ///if during the election, we have been traversed already
    ///I need those to be initialized at compile time with the value I want. If i put them in the union with the others, I cant do that
    ///And it will initialize them with junk. 
    bool traversed_election = false;
    int back_up_update = 0;
    int previous_update = 0;
    union{
        bool initiate_backup_update;
        
        int ph_count;
    };
    
    ///truck details
    struct truck_information{
    public:
        std::string pathLength_ = "594";
        std::string nSensor_ = "75";
        std::string nReparation_ = "0";
        std::string fuelConsumption_ = "26";
        std::string yearOfConstruction_ = "2013";
        std::string horsePower_ = "500";
        std::string mileage_ = "20";
    } truckInformation;
    
    
    
    ///stores all the actors handling different connections
    std::vector<caf::actor> vConnectionMultiplexer;//WIP
public:
    bool isMasterConnection();
    std::string getName();
    int32_t getId();
    int32_t getFrontId();
    uint16_t getPort();
    const char* getHostC();
    std::string getHost();
    float getSpeed();
    std::string getPreviousHost();
    uint16_t getPreviousPort();
    std::string getBackUpHost();
    uint16_t getBackUpPort();
    uint32_t getProcessID();
    std::string getPossibilityToBeMaster();
    
    
    void setMasterConnection(bool res);
    void setName(std::string name);
    void setId(int32_t id);
    void setFrontId(int32_t id);
    void setSpeed(float speed);
    void setPort(uint16_t port);
    void setHost(std::string host);
    void setPreviousHost(std::string host);
    void setPreviousPort(uint16_t port);
    void setBackUpHost(std::string host);
    void setBackUpPort(uint16_t port);
    void setProcessId(uint32_t processID);
    void setPossibilityToBeMaster(std::string possibility);
    
private:
    ///name of the truck
    std::string sName_ = "Name";
    ///Id of the truck
    int32_t iId_;
    ///front id, useless, should be removed
    int32_t iFrontId_;
    ///current speed
    float fSpeed = 0;
    ///Port of the server of this truck
    uint16_t uPort;
    ///Ip address of the server of this truck
    std::string sHost = "localhost";
    ///port of the truck in front of this truck
    uint16_t uPreviousTruckPort = 0;
    ///host of the truck in front of this truck
    std::string sPreviousTruckHost_ = "";
    ///port of the truck 2 trucks in front of this
    ///it will be used as a back up in case the guy in front fucks up
    uint16_t uBackUpPort = 0;
    ///host of the truck 2 trucks in front of this
    ///it will be used as a back up in case the guy in front fucks up
    std::string sBackUpHost_ = "";
    ///We store if we are connected directly to the master
    bool bMasterConnection; //are we connected to the master directly?
    ///The process ID is the specific ID that the truck owns and it's decided randomnly. This is used to define which one will be the new master.
    ///The other ID on the other hand, is used for the other operations
    uint32_t processID_;
    
    std::string masterPossibility_;
};

/*---------Behaviors-------------*/
///behavior handling the truck
///This refers to a normal truck
///the state is saved in the struct Truck
caf::behavior truck(caf::stateful_actor<Truck>* self);

///behavior handling the truck
///This refers to the truck when it becomes a master
///the state is saved in the struct Truck
caf::behavior master(caf::stateful_actor<Truck>* self);



/*---------Spawners-------------*/



///Spawns a truck with s given name and connects it to a given server. It also spawns a middleman which comunicates with the web interface.
///KEEP IN MIND THE PORT OF THE WEB SERVER IS OWN_PORT+1
///@param
/// system - system used to spawn the truck
/// name - name of the truck
/// host - ip of the server we want to connect to
/// port - port of the server we want to connect to
/// own_port - own port for the client
/// @return
/// actor handling the TRUCK
caf::actor spawnNewTruck(caf::actor_system& system, std::string name, std::string host, uint16_t port, uint16_t own_port);

///Spawns a master with s given name and connects it to a given server. It also spawns a middleman which comunicates with the web interface.
///KEEP IN MIND THE PORT OF THE WEB SERVER IS PORT+1
///@param
/// system - system used to spawn the truck
/// name - name of the truck which is the buddy of the master
/// host - ip of the server
/// port - port of the server
/// @return
/// actor handling the TRUCK
caf::actor spawnNewMaster(caf::actor_system& system,std::string name, std::string host , uint16_t port);

///Spawns a new client without truck.
///@param
/// system - system used to spawn the truck
/// name - name of the client
/// host - ip of the server to connect to
/// port - port of the server to connect to
/// own_port - port of the server of the truck
/// buddy - the truck to which we want to associate the client
/// @return
/// 0 - no errors
/// - 1 - errors
int spawnNewClient(caf::actor_system& system,std::string name, std::string host , uint16_t port, uint16_t own_port, const caf::actor& buddy);





/*---------Truck master server-------------*/


///behavior handling the master and its connections with the other trucks
///@param
/// self - broker itself
/// hdl - connection handler
/// buddy - truck  which we are associated to
caf::behavior TruckServerMaster(caf::io::broker *self, caf::io::connection_handle hdl, const caf::actor& buddy);

///Temp server used to handle situations in which we are not connected to any trucks
///@param
/// self - broker itself
/// buddy - truck  which we are associated to
caf::behavior temp_master_server(caf::io::broker* self, const caf::actor& buddy);

///behavior handling the connection between the master and the last truck in the platoon
///This is still a work in progress
///@param
/// self - broker itself
/// hdl - connection handler
/// buddy - truck  which we are associated to
caf::behavior TruckMasterClient(caf::io::broker *self, caf::io::connection_handle hdl, const caf::actor& buddy);

///WIP
///This is a WIP and it will allow the master to have multiple connections. Basically, this function here handles one connection, but it can be used as a bridge between the main master and this truck here
///@param
/// self - broker itself
/// hdl - connection handler
/// buddy - truck  which we are associated to
caf::behavior TruckMasterMultiplexer(caf::io::broker *self, caf::io::connection_handle hdl, const caf::actor& buddy);


/*---------Truck normal server-------------*/




///behavior handling the connection between two normal trucks. The truck behind this one will be connected to this broker here
///@param
/// self - broker itself
/// hdl - connection handler
/// buddy - truck  which we are associated to
caf::behavior TruckServer(caf::io::broker *self, caf::io::connection_handle hdl,const caf::actor& buddy);

///Temp server used to handle situations in which we are not connected to any trucks
///@param
/// self - broker itself
/// buddy - truck  which we are associated to
caf::behavior temp_server(caf::io::broker *self,const caf::actor& buddy);


///This is technically a server. However, this is used as a placeholder when a new truck wants to join.
///In poor words, this function is used to switch the position of the two trucks. Check the github for more information. I dont want to flood the code with comments
///@param
/// self - broker itself
/// hdl - connection handler
/// buddy - truck  which we are associated to
caf::behavior TruckSwitchRoutine(caf::io::broker *self, caf::io::connection_handle hdl,const caf::actor& buddy);


/*---------Client-------------*/

///behavior handling the connection between two normal trucks.THis broker will be connected to the server of the truck in front
///@param
/// self - broker itself
/// hdl - connection handler
/// buddy - truck  which we are associated to
caf::behavior TruckClient(caf::io::broker *self, caf::io::connection_handle hdl, const caf::actor& buddy);





///Server used to communicate with the web interface
///@param
/// self - broker itself
/// buddy - truck  which we are associated to
caf::behavior server(caf::io::broker* self,const caf::actor & buddy);


///Server used to communicate with the web interface
///@param
/// self - broker itself
/// buddy - truck  which we are associated to
caf::behavior connection_worker(caf::io::broker* self, caf::io::connection_handle hdl,const caf::actor & buddy);

///Sets the information related to the truck, in addition calculates the probability that the truck has to connect a master
///@param[in]
///pathLength  -> length of the path the truck has to follow
///nSensor -> number of sensors
///nReparation ->amount of the reparations
///fuelConsumption ->the fuel consumption
///yearOfConstruction ->year of construction
///horsePower ->amount of horse powers
///mileage ->amount of kms done
void setTruckInformation(std::string pathLength,
                         std::string nSensor,
                         std::string nReparation,
                         std::string fuelConsumption,
                         std::string yearOfConstruction,
                         std::string horsePower,
                         std::string mileage,const caf::actor& buddy);

void calculateTruckProbability(const caf::actor& buddy);

#endif /* Truck_hpp */
