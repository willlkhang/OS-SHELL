#ifndef SIGNAL_PROCESSOR_H
#define SIGNAL_PROCESSOR_H

void doing_nothing_method(int signal);

void sigchld_processor(int signal);

void activate_signal_handlers();

#endif //SIGNAL_PROCESSOR_H