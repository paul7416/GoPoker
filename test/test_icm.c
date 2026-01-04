#include"icm.h"
int main() {
    // Example: 3 players, typical SNG payouts (50%, 30%, 20%)
    int stacks[] = {3000, 3000, 5000, 7000};
    double payouts[] = {0.50, 0.30, 0.20};
    double ev[4];
    int n = 4;
    int no_payouts = 3;
    
    icm(stacks, n, payouts, 3, ev);
    
    printf("Stacks: ");
    for (int i = 0; i < n; i++) printf("%d ", stacks[i]);
    printf("\n");
    
    printf("Payouts: ");
    for (int i = 0; i < no_payouts; i++) printf("%.0f%% ", payouts[i] * 100);
    printf("\n\n");
    
    printf("ICM Equity:\n");
    double total_ev = 0;
    for (int i = 0; i < n; i++) {
        printf("  Player %d: %.4f (%.2f%%)\n", i + 1, ev[i], ev[i] * 100);
        total_ev += ev[i];
    }
    printf("\nTotal: %.4f (should be 1.0)\n", total_ev);
    
    // Chip EV for comparison
    printf("\nChip EV (for comparison):\n");
    double chip_total = 0;
    for(int i = 0; i < n; i++)
    {
        chip_total += stacks[i];
    }
    for (int i = 0; i < n; i++) {
        printf("  Player %d: %.2f%%\n", i + 1, (stacks[i] / chip_total) * 100);
    }
    return 0;
}
