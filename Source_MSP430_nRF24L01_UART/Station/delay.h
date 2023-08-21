#ifndef DELAY_H_
#define DELAY_H_

#define CYCLES_PER_US 8L // depends on the CPU speed
#define CYCLES_PER_MS (CYCLES_PER_US * 1000L)

#define delay_us(x) __delay_cycles((x * CYCLES_PER_US))
#define delay_ms(x) __delay_cycles((x * CYCLES_PER_MS))

#endif /* DELAY_H_ */
