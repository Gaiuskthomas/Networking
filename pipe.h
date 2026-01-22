#ifndef PIPE_H
#define PIPE_H

#define MAXMESSAGEDATA 4096

/* Message format used between processes */
struct message {
    int message_type;                    // Message type (not used much here)
    int message_length;                  // Length of valid data
    char message_data[MAXMESSAGEDATA];   // Message data
};

/* Header contains type + length */
#define MESGHDRSIZE (sizeof(int) * 2)

#endif