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

///THis is just convenience, you can change this depending on the size of the platoon
using truck_quantity = int32_t;

#endif /* defines_h */
