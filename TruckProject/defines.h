//
//  defines.h
//  Truck2
//
//  Created by Luca on 10/05/21.
//









#ifndef defines_h
#define defines_h

///Yeah, here I honestly fucked up.
/// I dont remember which one I use, so I kept them anyway
///It's 01.23 Im not going to check now. Fuck that
#define MAX_TRUCKS 64
#define MAX_TRUCK 64




#define MASK_FIRST_HALF(x) x & 0b11111111111111110000000000000000
#define SHIFT_TO_FIRST_BITS(x) x>>16
#define MASK_SECOND_HALF(x) x & 0xFFFFFFFF>>16


///Seconds to wait before reconnection
#define TIME_FOR_RECONNECTION 1
///This is bullshit
#define SERVER_PORT 3232


///The definitions for the type of selection algorithm
///Now, this is cool that we can define what type of election algortihm is chosen, however this is a thing that should be equal for every truck.
///Actually, no it should not.
///Every truck can have a different type. OMG this is cool, I didnt think of that
#define RING 1
#define NORMAL 2

#define RANDOM_PROCESS_ID uint32_t(std::rand()/((RAND_MAX + 1u)/ MAX_TRUCKS))




///THis is just convenience, you can change this depending on the size of the platoon
using truck_quantity = int32_t;

#endif /* defines_h */
