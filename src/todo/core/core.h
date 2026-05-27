/**
 * @file core.h
 *
 * @author Adnan Faize <adnanfaize@gmail.com>
 */

#ifndef CORE_H_
#define CORE_H_

/**
 * Can be initialized only once.
 */
extern bool core_init(void);
/**
 * Exits from the core. Clears all memory if any allocated.
 */
extern bool core_exit(void);

#endif // APP_CORE_H_
