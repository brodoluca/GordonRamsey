#include "Truck.hpp"

///This file contains the implementation of the two actors which acts as actor and master
///all the clients, servers and so on pass through these to do every action.
///They also store save the state of the trucks, so speed, name bla bla (check the struck for more infos)
///It also functions as a glue between client and server

caf::behavior truck(caf::stateful_actor<Truck>* self){
    //no op
    //initialization is done through a behavior
    return{
        
        ///Initializes the actor with a name and save the reference to the client that spawned it (be careful, if spawned by something else will save the reference to that thing)
        ///@param[in]
        /// name - name of the actor
        [=](initialize_atom,std::string name) {
            self->state.setName(name);
            std::cout<<self->state.getName() + " has been spawned \n";
            self->state.client = self->current_sender();
        },
        
        ///Initializes the actor with a name and save the reference to the client that spawned it (be careful, if spawned by something else will save the reference to that thing)
        ///it also spawns a server with a given port. saves the reference to that server.
        ///@param[in]
        /// name - name of the actor
        ///     port - port thanks to which we can spawn the server
        [=](initialize_atom,std::string name, uint16_t port) {
            self->state.setName(name);
            std::cout<<self->state.getName() + " has been spawned \n";
            self->state.client = self->current_sender();
            self->state.setPort(port);
            auto server = self->home_system().middleman().spawn_server(temp_server, port, caf::actor_cast<caf::actor>(self));
            if (!server) {
                std::cerr << "failed to spawn "<< name << "'s client: " << to_string(server.error()) << "\n"<< std::endl;
            }
            self->attach_functor([=](const caf::error& reason) {
                std:: cout << "["+ self->state.getName() +"]: There is an error. REASON = "<< to_string(reason);
                self->send_exit(caf::actor_cast<caf::actor>(self->state.server),caf::exit_reason::remote_link_unreachable);
            });
            send_as(*server, self, set_server_atom_v);
        },
        
        ///Initializes the actor WITHOUT    a name and save the reference to the client that spawned it (be careful, if spawned by something else will save the reference to that thing)
        ///it also spawns a server without a server
        ///@param[in]
        ///     port - port saved in case a server needs to be spawned later
        [=](initialize_atom,uint16_t port) {
            std::cout<<self->state.getName() + " has been spawned again. This time, without server \n";
            self->state.client = self->current_sender();
            self->state.setPort(port);
        },
    
        [=](available_to_count) {
            return (self->state.initiated_switcheroo == 0)||(self->state.tqPlatoon == 1);
        },
        
        ///returns size of the platoon
        ///@param[in]
        ///none
        ///@return
        ///size of the platoon
        [=](get_truck_numbers_atom) {
            return self->state.tqPlatoon;
        },
        
        ///check wheter connection is a master connection or not
        ///@param[in]
        ///none
        ///@return
        ///is either 1 (true) or zero(false)
        [=](is_master_atom) {
            return self->state.isMasterConnection();
        },
        
        ///@param[in]
        ///none
        ///@return
        ///the Port as uint16_t
        [=](get_port_atom) {
            return self->state.getPort();
        },
        
        ///@param[in]
        ///none
        ///@return
        ///the ip associated to this truck as a C string (char*)
        [=](get_host_atom) {
            return self->state.getHostC();
        },
        
        ///@param[in]
        ///none
        ///@return
        ///the id of the truck
        [=](which_id_atom) {
            return self->state.getId();
        },
        
        ///@param[in]
        ///none
        ///@return
        ///the port(uint16) and the host (std::string) associated to this truck as a pair
        [=](get_host_port_atom) {
            return std::make_pair(int32_t(self->state.getPort()),self->state.getHost());
        },

        ///@param[in]
        ///none
        ///@return
        ///the port(uint16) and the host (std::string) associated to the truck in front as a pair
        [=](is_switcheroo_atom) {
            return self->state.initiated_switcheroo;
        },
        ///@param[in]
        ///none
        ///@return
        ///the port(uint16) and the host (std::string) associated to the truck in front as a pair
        [=](set_switcheroo_atom, int isSwitcheroo) {
             self->state.initiated_switcheroo = isSwitcheroo;
        },
        ///@param[in]
        ///none
        ///@return
        ///the port(uint16) and the host (std::string) associated to the truck in front as a pair
        [=](get_port_host_previous_atom) {
            return std::make_pair(int32_t(self->state.getPreviousPort()),self->state.getPreviousHost());
        },
        
        ///@param[in]
        ///none
        ///@return
        ///the port(uint16) and the host (std::string) associated to the back up truck as a pair
        [=](get_port_host_back_up_atom) {
            return std::make_pair(int32_t(self->state.getBackUpPort()),self->state.getBackUpHost());
        },
        
        ///Assigns a new id to the truck and updates back by subtracting one. in addition, asks to the client if its a master connection
        ///@param[in]
        ///the id as a int32_t
        ///@return
        ///none
        [=](get_new_id_atom, int32_t newID) {
            self->state.setId(newID);
            self->send(caf::actor_cast<caf::actor>(self->current_sender()), is_master_atom_v);
            self->send(caf::actor_cast<caf::actor>(self->state.server), update_id_cascade_v, int32_t(self->state.getId()-1));
        },
    
        ///Us'ed to save the type of conneciton.
        ///@param[in]
        ///bool isMaster
        ///True if it's master
        ///False if not
        ///@return
        ///none
        [=](set_master_connection_atom, bool isMaster) {
            self->state.setMasterConnection(isMaster);
//            if (self->state.tqPlatoon <= truck_quantity(1)) {
            if(self->state.tqPlatoon == 1)
                self->delayed_send(caf::actor_cast<caf::actor>(self->state.client),std::chrono::milliseconds(100),master_connect_to_my_server_atom_v);
//            }
        },
        
        ///Updates the port and the host of associated to this truck
        ///@param[in]
        ///uint16_t new port - new port to associate this truck with
        ///host - ip to associate this truck with
        ///@return
        ///none
        [=](update_port_host_atom, uint16_t newPort, std::string newHost) {
            self->state.setPort(newPort);
            self->state.setHost(newHost);
        },
        
        ///Receives the commands from the client and interprets them. It also sends the command to the truck behind
        ///@param[in]
        ///int32_t command -  the command that needs to be followed
        ///@return
        ///none
        [=](get_new_command, int32_t command) {
            switch (static_cast<commands>(command)) {
                case commands::stop:
                    self->state.setSpeed(0);
                    std::cout << self->state.getName()+" stops: " + std::to_string(self->state.getSpeed()) +"\n";
                    if (self->state.server)
                        self->anon_send(caf::actor_cast<caf::actor>(self->state.server), send_new_command_atom_v, static_cast<uint32_t>(command));
        
                    break;
                case commands::start:
                    self->state.setSpeed(100);
                    std::cout << self->state.getName()+" starts: " + std::to_string(self->state.getSpeed()) +" \n";
                    if (self->state.server)
                        self->anon_send(caf::actor_cast<caf::actor>(self->state.server), send_new_command_atom_v,static_cast<uint32_t>(command));
                    break;  
                case commands::accellerate:
                    self->state.setSpeed(self->state.getSpeed()+10);
                    std::cout << self->state.getName()+" accellerates: " + std::to_string(self->state.getSpeed()) +" \n";
                    if (self->state.server)
                        self->anon_send(caf::actor_cast<caf::actor>(self->state.server), send_new_command_atom_v, static_cast<uint32_t>(command));
                    break;
                case commands::decellerate:
                    self->state.setSpeed(self->state.getSpeed()-10);
                    std::cout << self->state.getName()+" decellerates: " + std::to_string(self->state.getSpeed()) +" \n" ;
                    if (self->state.server)
                        self->anon_send(caf::actor_cast<caf::actor>(self->state.server), send_new_command_atom_v, static_cast<uint32_t>(command));
                    break;
                
                default:
                    break;
            }
        },
        
        ///Routine to become the master. Basically, it becomes the master behavior, sets its id as the biggest possible number and adverst the server to become the master as well
        ///@param[in]
        ///none
        ///@return
        ///none
        [=](become_master_atom){
            std::cout << "["+ self->state.getName() +"]: I am the new master\n";
            self->become(master(self));
            self->state.setId(MAX_TRUCKS);
            self->send(caf::actor_cast<caf::actor>(self->state.server), become_master_atom_v);
        },
        
        ///Sent by the server, it stores the pointer to the server to be able to send messages to it.
        ///Keep in mind, it needs to be casted as an actor before use
        ///@param[in]
        ///none
        ///@return
        ///none
        [=](set_server_atom){
            self->state.server = self->current_sender();
            self->attach_functor([=](const caf::error& reason) {
                std:: cout << "Server down";
                self->send_exit(caf::actor_cast<caf::actor>(self->state.server),caf::exit_reason::remote_link_unreachable);});
        },

        ///Closes the connection with the client and allows the old switcheroo.
        ///Basically, since the connection to the client is done, we can create a new client that connects to the other truck
        ///@param[in]
        ///none
        ///@return
        ///none
        [=](close_client_connection_atom) {
            self->anon_send(caf::actor_cast<caf::actor>(self->state.client), close_connection_atom_v);
            self->delayed_anon_send(caf::actor_cast<caf::actor>(self->current_sender()),std::chrono::milliseconds(100), allow_switcheroo_atom_v);
        },
        
        ///Spawns a a server with a given port
        ///@param[in]
        ///uint16_t port .- port to spawn the server with
        ///@return
        ///none
        [=](spawn_server_atom, uint16_t port) {
            auto server = self->home_system().middleman().spawn_server(temp_server, port, caf::actor_cast<caf::actor>(self));
            self->attach_functor([=](const caf::error& reason) {
                std:: cout << "["+ self->state.getName() +"]: There is an error. REASON = "<< to_string(reason);
                self->send_exit(caf::actor_cast<caf::actor>(self->state.server),caf::exit_reason::remote_link_unreachable);
            });
        },
        
        ///increments the size of the platoon and also sends that to the client and to the server
        ///@param[in]
        ///none
        ///@return
        ///
        ///none
        [=](increment_number_trucks_atom){
            self->state.tqPlatoon+= 1;
//            self->anon_send(caf::actor_cast<caf::actor>(self->state.client), update_truck_numbers_atom_v,self->state.tqPlatoon);
//            self->delayed_anon_send(caf::actor_cast<caf::actor>(self->state.client),std::chrono::milliseconds(1000), update_truck_numbers_atom_v, self->state.tqPlatoon);
            std::cout << "["+ self->state.getName() + "]: " +"Platoon after incrementing is: "<<self->state.tqPlatoon<<"\n";
//            self->state.initiate_update = true;
        },
    
        ///Decreases the number of trucks and informs the truck behind and in front
        ///@param[in]
        ///none
        ///@return
        ///none
        [=](decrease_number_trucks_atom){
            if(self->state.tqPlatoon > 0)
                self->state.tqPlatoon-=1;
            std::cout << "["+ self->state.getName() + "]: " +" Platoon after decreasing is : "<<self->state.tqPlatoon<<"\n";
//            self->delayed_anon_send(caf::actor_cast<caf::actor>(self->state.server),std::chrono::milliseconds(10), update_truck_numbers_atom_v, self->state.tqPlatoon);
//            self->delayed_anon_send(caf::actor_cast<caf::actor>(self->state.client),std::chrono::milliseconds(100), update_truck_numbers_atom_v, self->state.tqPlatoon);
//            self->state.initiate_update = true;
        },
    
        ///Decreases the number of trucks BY A CERTAIN QUANTITY and informs the truck behind and in front
        ///@param[in]
        ///truck_quantity quantity - the quantity to decreases the platoon of
        ///@return
        ///none
    
        [=](decrease_number_trucks_atom, truck_quantity quantity){
            if(self->state.tqPlatoon >0)
                self->state.tqPlatoon-=quantity;
            std::cout << "["+self->state.getName()+"]:"+" Platoon after decreasing by a certain amount is : "<<self->state.tqPlatoon<<"\n";
//            self->delayed_anon_send(caf::actor_cast<caf::actor>(self->state.client),std::chrono::milliseconds(10), update_truck_numbers_atom_v, self->state.tqPlatoon);
//            self->state.initiate_update = true;
        },

        ///sets a new quantity of trucks
        ///@param[in]
        ///truck_quantity quantity - the new size of the paltoon
        ///@return
        ///none
        [=](update_truck_numbers_atom, truck_quantity a ) {
            self->state.tqPlatoon = a;
            if(self->state.initiate_update){
                std::cout << "["+self->state.getName()+"]:"+" Platoon is now : " << self->state.tqPlatoon << std::endl;
//               self->delayed_anon_send(caf::actor_cast<caf::actor>(self->state.server),std::chrono::milliseconds(10), update_truck_numbers_atom_v, self->state.tqPlatoon);
                self->anon_send(caf::actor_cast<caf::actor>(self->state.client), update_truck_numbers_atom_v,self->state.tqPlatoon);
            }else{
                self->state.initiate_update = false;
                std::cout << "["+self->state.getName()+"]:"+" Finished updating: " << self->state.tqPlatoon << std::endl;
            }
        },
        
        ///sets a new quantity of trucks
        ///@param[in]
        ///uint32_t a - the new size of the paltoon
        ///@return
        ///none
        [=](update_truck_numbers_atom, uint32_t a ) {
            self->state.tqPlatoon = a;
            if(!self->state.initiate_update){
                std::cout << "["+self->state.getName()+"]:"+" Platoon is now(int32_t)): " << self->state.tqPlatoon << std::endl;
//               self->delayed_anon_send(caf::actor_cast<caf::actor>(self->state.server),std::chrono::milliseconds(10), update_truck_numbers_atom_v, self->state.tqPlatoon);
                self->anon_send(caf::actor_cast<caf::actor>(self->state.client), update_truck_numbers_atom_v,self->state.tqPlatoon);
            }else{
                self->state.initiate_update = false;
                std::cout << "["+self->state.getName()+"]:"+" Finished updating: " << self->state.tqPlatoon << std::endl;
            }
        },
        ///Updates the port and host of the truck in front
        ///Also tells the server to update the guy behind this truck
        ///Redundancy is important in this stage. This is the only anti fail machanism we have
        ///@param[in]
        ///uint16_t port - new port
        /// std::string host - new ip
        ///@return
        ///none
        [=](update_port_host_previous_atom, uint16_t port, std::string host){
            self->state.setPreviousHost(host);
            self->state.setPreviousPort(port);
            std::cout << "["+ self->state.getName() + "]: " +" Previous truck updated :" << self->state.getPreviousHost() << ", port: " << self->state.getPreviousPort() << std::endl ;
        
//            self->send(caf::actor_cast<caf::actor>(self->state.server), update_back_up_atom_v);
            self->delayed_send(caf::actor_cast<caf::actor>(self->state.server), std::chrono::milliseconds(10), update_port_host_previous_atom_v);
        },
        ///Updates the port and host of theback up truck
        ///@param[in]
        ///uint16_t port - new port
        /// std::string host - new ip
        ///@return
        ///none
        [=](update_back_up_atom, uint16_t port, std::string host){
            self->state.setBackUpHost(host);
            self->state.setBackUpPort(port);
            std::cout << "["+ self->state.getName() + "]: " +" BackUp truck updated :" << self->state.getBackUpHost() << "-" << self->state.getBackUpPort() << std::endl ;
            self->send(caf::actor_cast<caf::actor>(self->state.server), update_back_up_atom_v);
        },
        
        ///When the truck in front leaves or dies we connect to the truck in front of us and we kill the other client.
        ///No handling of errors
        ///@param[in]
        ///none
        ///@return
        ///none
        [=](truck_left_or_dead_atom) {
            auto im = self->home_system().middleman().spawn_client(TruckClient, self->state.getBackUpHost(), self->state.getBackUpPort(), self);
            if(!im)
                std::cerr << "failed to spawn "<< self->state.getName() << "'s client: " << to_string(im.error()) <<"\n"<< std::endl;
            self->send_exit(caf::actor_cast<caf::actor>(self->current_sender()), caf::exit_reason::remote_link_unreachable);
        },
        
        ///This is called by the switcheroo function. Basically, it creates a client that connects to the truck in front of the truck we want to connect to
        ///@param[in]
        ///none
        ///@return
        ///none
        [=](create_new_client_connection_previous_hostport_atom) {
            auto result = spawnNewClient(self->home_system(), self->state.getName(), self->state.getPreviousHost(), self->state.getPreviousPort(), self->state.getPort(), caf::actor_cast<caf::actor>(self));
            if(result == 0){
                std::cout << "["+ self->state.getName() +"]: new client created and successfully connected : " <<self->state.getPreviousHost() <<self->state.getPreviousPort() << std::endl;
            }else{
                std::cout << "["+ self->state.getName() +"]: Impossible to create new client" << std::endl;
            }
        },
        
        ///This is called when there is a necessity to count the truck in the platoon
        ///THis is the starting point. From this, the token will pass through all the nodes in the network
        ///@param[in]
        ///none
        ///@return
        ///none
        [=](count_trucks_atom) {
            self->send(caf::actor_cast<caf::actor>(self->state.client), count_trucks_atom_v,
                       std::make_pair(uint32_t(self->state.getId()), uint32_t(1)) );
            std::cout << "["+ self->state.getName() +"]: Started Counting" << std::endl;
        },
        
        ///This is called when there is a necessity to count the truck in the platoon
        ///This is the behavior called to pass the token to the next node
        ///@param[in]
        ///std::pair<uint32_t, uint32_t> pStopIdCount -> the pair containing the stop id(The id of the truck that initialized the counting)
        ///and the counter
        ///@return
        ///none
        [=](count_trucks_atom, std::pair<uint32_t, uint32_t> pStopIdCount) {
            if (pStopIdCount.first == self->state.getId()) {
                std::cout << "["+ self->state.getName() +"]: Finished Counting" << std::endl;
                self->state.tqPlatoon = pStopIdCount.second;
                self->state.initiate_update = true;
                self->send(self,update_truck_numbers_atom_v,self->state.tqPlatoon);
            }else{
                pStopIdCount.second = pStopIdCount.second+1;
                
                std::cout << "["+ self->state.getName() +"]: Counting from:" << pStopIdCount.first<< std::endl;
                self->send(caf::actor_cast<caf::actor>(self->state.client), count_trucks_atom_v, pStopIdCount);
            }
        },
        
        ///saves the current sender as client
        ///@param[in]
        ///none
        ///@return
        ///none
        [=](set_client_atom) {
            self->state.client = self->current_sender();
        },
        
        /*---------------------------USELESS --------------------*/
        
        [=](cascade_port_host_atom, uint16_t newPort, std::string newHost, truck_quantity stopID) {
            if (stopID == self->state.getId()) {
//                place holder
                std::cout << "Hey, that's it\n";
            }else{
                self->anon_send(caf::actor_cast<caf::actor>(self->state.server), cascade_port_host_atom_v, newPort, newHost, stopID);
            }
        },
        [=](update_master_atom, std::string host, uint16_t port) {
            auto impl = self->home_system().middleman().spawn_client(TruckClient, host, port, caf::actor_cast<caf::actor>(self->address()));
        },
        [=](update_id_behind_atom) {
//            self->anon_send(caf::actor_cast<caf::actor>(self->state.server), update_id_behind_atom_v,self->state.getId()-1);
        },
        [=](set_front_id, int32_t newID) {
            self->state.setFrontId(newID);
        },
        [=](you_are_master_atom) {
            
        },
        
        [=](tell_back_im_master_atom) {
//            self->send(caf::actor_cast<caf::actor>(self->state.server), tell_back_im_master_atom_v);
        },
        [=](which_front_id_atom) {
            return self->state.getFrontId();
        },
        
        /*---------------------------EXPERIMENTAL--------------------*/
        
        
        ///Adds new connection. This is still experimantal
        ///@param
        ///uint16_t port - new port
        /// std::string host - new ip
        ///@return
        ///none
        [=](add_connection_atom, const caf::actor& newConnection) {
            self->state.vConnectionMultiplexer.push_back(std::move(newConnection));
            std::cout << "["+ self->state.getName() + "]: " +" New connection added. Number of connections : "<<self->state.vConnectionMultiplexer.size()<<std::endl;
        },
        
    };
}


caf::behavior master(caf::stateful_actor<Truck>* self){
    self->state.ph_count = 0;
    return {
        ///This is here mainly for sanity checks and debugs
        ///@param
        ///none
        ///@return
        ///none
        [=](become_master_atom){
            std::cout << "["+ self->state.getName() + "]: I am already the master\n";
        },
        
        ///stores the reference of the sender as a server and attaches it a function so that in case there's an error it dies
        ///@param
        ///none
        ///@return
        ///none
        [=](set_server_atom) {
            self->state.server = self->current_sender();
            self->attach_functor([=](const caf::error& reason) {
                std:: cout << "Server down";
                self->send_exit(caf::actor_cast<caf::actor>(self->state.server),caf::exit_reason::remote_link_unreachable);
            });
        },
            
        ///Assign the id to the truck behind as my id - 1
        ///@param
        ///none
        ///@return
        ///none
        [=](assign_id_atom){
            return self->state.getId() - 1;
        
        },
        
        ///@param
        ///none
        ///@return
        ///a pair with my port and my ip address
        [=](get_host_port_atom) {
            return std::make_pair(int32_t(self->state.getPort()),self->state.getHost());
        
        ///from here all of these functions have the same functionalities as before
        ///In hindsight, dividing the actor in master and truck was a mistake
        ///But semantically (and also in my head) made sense. In my head,  thinking of the truck and the master as two separate entities helped me
            
        },[=](update_id_behind_atom) {
//            self->anon_send(caf::actor_cast<caf::actor>(self->state.server), update_id_behind_atom_v,uint32_t{self->state.getId()-uint32_t(1)});
        },
        [=](tell_back_im_master_atom) {
            self->send(caf::actor_cast<caf::actor>(self->state.server), tell_back_im_master_atom_v);
        },
        ///This is useless
        [=](increment_number_trucks_atom, truck_quantity platoon) {
            self->state.tqPlatoon = platoon;
        },
        
//        ///increments the size of the platoon and also sends that to the client and to the server by
//        ///@param[in]
//        ///none
//        ///@return
//        ///none
//        [=](increment_number_trucks_atom) {
//            self->state.tqPlatoon = self->state.tqPlatoon+1;
//        },
        
        ///returns size of the platoon
        ///@param[in]
        ///none
        ///@return
        ///size of the platoon
        [=](get_truck_numbers_atom) {
            return self->state.tqPlatoon;
        },
        [=](increment_number_trucks_atom, uint32_t newPlatoon){
            self->state.tqPlatoon += newPlatoon;
            std::cout << "[MASTER "+ self->state.getName() +"]: Platoon is now  : "<<self->state.tqPlatoon<<"\n";
        },
        ///increments the size of the platoon and also sends that to the client and to the server by
        ///@param[in]
        ///none
        ///@return
        ///none
        [=](increment_number_trucks_atom){
            self->state.tqPlatoon += 1;
            std::cout << "[MASTER "+ self->state.getName() +"]: Platoon is now  : "<<self->state.tqPlatoon<<"\n";
//            self->state.initiate_update = true;
        },
        
        ///sets a new quantity of trucks
        ///@param[in]
        ///truck_quantity quantity - the new size of the paltoon
        ///@return
        ///none
        [=](update_truck_numbers_atom, truck_quantity a ) {
            self->state.tqPlatoon = a;
            if(!self->state.initiate_update){
                std::cout << "[MASTER "+self->state.getName()+"]:"+" Platoon is now : " << self->state.tqPlatoon << std::endl;
                self->delayed_anon_send(caf::actor_cast<caf::actor>(self->state.client),std::chrono::milliseconds(100), update_truck_numbers_atom_v, self->state.tqPlatoon);
            }else{
                self->state.initiate_update = false;
                std::cout << "[MASTER"+self->state.getName()+"]:"+" Finished updating: " << self->state.tqPlatoon << std::endl;
            }
        },
        
        ///sets a new quantity of trucks. The difference with the one above is that this une uses a uint32_t as a parameter
        ///@param[in]
        ///uint32_t quantity - the new size of the paltoon
        ///@return
        ///none
        [=](update_truck_numbers_atom, uint32_t a ) {
            self->state.tqPlatoon = a;
            if(!self->state.initiate_update){
                std::cout << "[MASTER "+self->state.getName()+"]:"+" I updated the platoon: " << self->state.tqPlatoon << std::endl;
                self->delayed_anon_send(caf::actor_cast<caf::actor>(self->state.client),std::chrono::milliseconds(10), update_truck_numbers_atom_v, self->state.tqPlatoon);
            }else{
                self->state.initiate_update = false;
                std::cout << "[MASTER "+self->state.getName()+"]:"+" Finished updating: " << self->state.tqPlatoon << std::endl;
            }
        },
        
        ///@param[in]
        ///none
        ///@return
        ///the Port as uint16_t
        [=](get_port_atom) {
            return self->state.getPort();
        },
        
        ///Decreases the number of trucks by a certain quantity and informs the truck behind and in front
        ///@param[in]
        ///none
        ///@return
        ///none
        [=](decrease_number_trucks_atom, truck_quantity quantity){
            if(self->state.tqPlatoon != 0){
                self->state.tqPlatoon-=quantity;
            }
            std::cout << "[MASTER "+ self->state.getName() +"]: Platoon after decreasing by a certain amount is : "<<self->state.tqPlatoon<<"\n";
            
        },
        
        ///Adds new connection. This is still experimantal
        ///@param
        ///uint16_t port - new port
        /// std::string host - new ip
        ///@return
        ///none
        [=](add_connection_atom, const caf::actor& newConnection) {
            self->state.vConnectionMultiplexer.push_back(std::move(newConnection));
            std::cout << "[MASTER "+ self->state.getName() +"]: New connection added. Number of connections : "<<self->state.vConnectionMultiplexer.size()<<std::endl;
        },
        
        ///@param[in]
        ///none
        ///@return
        ///the port(uint16) and the host (std::string) associated to the truck in front as a pair
        [=](get_port_host_previous_atom) {
            return std::make_pair(int32_t(self->state.getPreviousPort()),self->state.getPreviousHost());
        },
        
        ///@param[in]
        ///none
        ///@return
        ///the port(uint16) and the host (std::string) associated to the back up truck as a pair
        [=](get_port_host_back_up_atom) {
            return std::make_pair(int32_t(self->state.getBackUpPort()),self->state.getBackUpHost());
        },
        
        ///It creates a new client with the port and host obtained before
        ///@param[in]
        ///none
        ///@return
        ///none
        [=](create_new_client_connection_previous_hostport_atom) {
            auto result = spawnNewClient(self->home_system(), self->state.getName(), self->state.getPreviousHost(), self->state.getPreviousPort(), self->state.getPort(), caf::actor_cast<caf::actor>(self));
            if(result == 0){
                std::cout << "[MASTER "+ self->state.getName() +"]: new client created and successfully connected" << std::endl;
            }else{
                std::cout << "[MASTER "+ self->state.getName() +"]: Impossible to create new client" << std::endl;
            }
        },
        
       
        
        ///This function is a bit weird. Basically, in case the size of the platoon is less than 2 (1 truck connected), the master will try to connect to the server of the truck that is connected to it. It closes the ring
        ///@param[in]
        ///,uint16_t port -> the port of the truck
        ///std::string host -> the ip of the truck
        ///@return
        ///none
        [=](master_connects_to_truck_behind,uint16_t port, std::string host){
            if(self->state.tqPlatoon <2){
                self->state.setPreviousHost(host);
                self->state.setPreviousPort(port);
                auto im = self->home_system().middleman().spawn_client(TruckMasterClient, self->state.getPreviousHost(), self->state.getPreviousPort(), self);
                if (!im) {
                    std::cerr << "failed to spawn "<< self->state.getName() << "'s client: " << to_string(im.error()) <<"\n"<< std::endl;
                }
            }
            
        },
    
        ///This is called when there is a necessity to count the truck in the platoon
        ///This is the behavior called to pass the token to the next node
        ///@param[in]
        ///std::pair<uint32_t, uint32_t> pStopIdCount -> the pair containing the stop id(The id of the truck that initialized the counting)
        ///and the counter
        ///@return
        ///none

        [=](count_trucks_atom, std::pair<uint32_t, uint32_t> pStopIdCount) {
            if (pStopIdCount.first == self->state.getId()) {
                std::cout << "[MASTER "+self->state.getName()+"]:"+" STOPPED counting "<< std::endl;
                self->state.initiate_update = false;
                self->state.tqPlatoon = pStopIdCount.second;
                self->send(self,update_truck_numbers_atom_v,self->state.tqPlatoon);
            }else{
                std::cout << "[MASTER "+self->state.getName()+"]:"+" counting "<< std::endl;
                pStopIdCount.second = pStopIdCount.second+1;
                self->delayed_send(caf::actor_cast<caf::actor>(self->state.client),std::chrono::milliseconds(10), count_trucks_atom_v, pStopIdCount);
                
            }
        },
        ///This is called when there is a necessity to count the truck in the platoon
        ///THis is the starting point. From this, the token will pass through all the nodes in the network
        ///@param[in]
        ///none
        ///@return
        ///none
        [=](count_trucks_atom) {
            self->send(caf::actor_cast<caf::actor>(self->state.client), count_trucks_atom_v,
                       std::make_pair(uint32_t(self->state.getId()), uint32_t(1)) );
            self->state.initiate_update = true;
//            std::cout << "WORKS";
        },
        [=](set_client_atom) {
            self->state.client = self->current_sender();
        },
        
        
        
        
        [=](get_new_id_atom, int32_t newID) {
        },
        [=](update_port_host_previous_atom, uint16_t port, std::string host){
        },
        [=](update_back_up_atom, uint16_t port, std::string host){
            if (self->state.getBackUpHost() == host && self->state.getBackUpPort() == port) {
                self->state.ph_count+=1;
                std::cout << "[MASTER "+ self->state.getName() + "]: " +" No need to update the back up truck"  << std::endl ;
                if (self->state.ph_count == 2 ) {
                    self->state.ph_count = 0;
                    std::cout << "[MASTER "+ self->state.getName() + "]: " +"However, I tell it back"  << std::endl ;
                    self->send(caf::actor_cast<caf::actor>(self->state.server), update_back_up_atom_v);
                }else if (self->state.ph_count > 2){
//                    no op
                    
                }
                
            }else{
                self->state.setBackUpHost(host);
                self->state.setBackUpPort(port);
                std::cout << "[MASTER "+ self->state.getName() + "]: " +" BackUp truck updated :" << self->state.getBackUpHost() << "-" << self->state.getBackUpPort() << std::endl ;
                self->send(caf::actor_cast<caf::actor>(self->state.server), update_back_up_atom_v);
            }
            
                       
            
            
        },
        ///Updates the port and host of the truck in front
        ///Also tells the server to update the guy behind this truck
        ///Redundancy is important in this stage. This is the only anti fail machanism we have
        ///@param[in]
        ///uint16_t port - new port
        /// std::string host - new ip
        ///@return
        ///none
        [=](update_port_host_previous_atom, uint16_t port, std::string host){
            if (self->state.getPreviousHost() == host && self->state.getPreviousPort() == port) {
                self->state.ph_count+=1;
            }else{
                self->state.setPreviousHost(host);
                self->state.setPreviousPort(port);
                std::cout << "[MASTER "+ self->state.getName() +"]: " +" Previous truck updated :" << self->state.getPreviousHost() << ", port: " << self->state.getPreviousPort() << std::endl ;
                self->delayed_send(caf::actor_cast<caf::actor>(self->state.server), std::chrono::milliseconds(10), update_port_host_previous_atom_v);
                self->state.ph_count = 0;
            }
            
            
//            self->send(caf::actor_cast<caf::actor>(self->current_sender()), update_back_up_atom_v);
            
        },
    };
}





/*
 *
 *      THis is the part relative to the struct. Here the setters and getters are defined. 
 *
 */



///
///   Getters
///
std::string Truck::getName(){
    return sName_;
}
int32_t Truck::getId(){
    return iId_;
}
int32_t Truck::getFrontId(){
    return iFrontId_;
}

float Truck::getSpeed(){
    return fSpeed;
}
std::string Truck::getHost(){
    return sHost;
}
uint16_t Truck::getPort(){
    return uPort;
}
const char* Truck::getHostC(){
    char* temp=new char(17);
    std::strcpy(temp, sHost.c_str());
    for(unsigned long i=sHost.length();i<17;i++)
        temp[i] = '-';
    return temp;
}

bool Truck::isMasterConnection(){
    return bMasterConnection;
}
std::string Truck::getPreviousHost(){
    return sPreviousTruckHost_;
}
uint16_t Truck::getPreviousPort(){
    return uPreviousTruckPort;
}
std::string Truck::getBackUpHost(){
    return sBackUpHost_;
}
uint16_t Truck::getBackUpPort(){
    return uBackUpPort;
}


///
///    Setters
///

void Truck::setName(std::string name){
    sName_ = std::move(name);
};
void Truck::setId(int32_t id){
    iId_ = id;
};
void Truck::setFrontId(int32_t id){
    iFrontId_ = (id == 1)? 64 : id;
};
void Truck::setSpeed(float speed){
    fSpeed = speed;
};

void Truck::setHost(std::string host){
    sHost = host;
}

void Truck::setPort(uint16_t port){
    uPort = port;
}

void Truck::setMasterConnection(bool res){
    bMasterConnection = res;
}

void Truck::setPreviousHost(std::string host){
    sPreviousTruckHost_ = host;
}

void Truck::setPreviousPort(uint16_t port){
    uPreviousTruckPort = port;
}
void Truck::setBackUpHost(std::string host){
    sBackUpHost_ = host;
}
void Truck::setBackUpPort(uint16_t port){
    uBackUpPort = port;
}

