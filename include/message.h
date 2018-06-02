/* message.h */

#define NMSG 10 /* Maximum number of messages per process */

// Message Structure
struct Message
{
  umsg32 message;
  struct Message *next;
};

extern umsg32 get_message(pid32);
extern status add_message(pid32, umsg32);
