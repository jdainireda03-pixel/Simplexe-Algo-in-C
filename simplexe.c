#include <stdio.h>
#define MAX_SLACK 10
#define MAX_VARS 10

struct simplexe {
    int m; //number of constraints
    int n; //number of variable (slack ones not included)
    double tableau[MAX_SLACK + 1][MAX_SLACK + MAX_VARS + 1]; // tableau including RHS and slack variables
};

//for these first ones i'll try to solve for max (min is a tad more complicated so it will take me more time)


//function to enter fill the table
void inputproblem(struct simplexe *s) {
    
    
    printf("Enter the number of constrains: ");
    scanf("%d", &s->m);
    
    printf("Enter the number of original variables: ");
    scanf("%d", &s->n);

    for (int i=0; i <= s->m; i++) //init the tab at 0 so when everything else is filled the one who don't are alr at 0
    {
        for (int j=0; j <= s->m + s->n ; j++) {
            s->tableau[i][j]=0;
        }
    }

    
    printf("Enter the coefficients of the target (equation economique): ");
    for (int i=0; i < s->n; i++) {
        scanf("%lf", &s->tableau[s->m][i]);
    }
    
    printf("Entrer the coefficients of the constraints (end with RHS): ");
    for (int i=0; i < s->m; i++)
    {
        printf("Constraint %d: ", i+1);
        for (int j=0; j < s->n ; j++) {
            scanf("%lf", &s->tableau[i][j]);
        }
        scanf("%lf", &s->tableau[i][s->n + s->m]); // adding the RHS skiping to the last column of each row
    }
    
    for(int i=0; i<s->m; i++) {
        s->tableau[i][s->n + i] = 1.0; //adding the slack vars coeff
    }
}

//function to print it
void print_tableau(struct simplexe *s) {
    printf("\n");
    for (int i=0; i <= s->m; i++) {
        if (i == s->m) {
            printf("-------------------------------------------\n");
        }
        for (int j=0; j <= s->m + s->n; j++) {
            if (j == s->m + s->n) {
                printf(" | ");
            }
            printf("   %.2f   ", s->tableau[i][j]);
        }
        printf("\n");
    }  
}

int isoptimal(struct simplexe *s) {
    for (int i=0; i < s->n + s->m; i++) {
        if (s->tableau[s->m][i] > 0 ) {
            return 0; //not an optimal solution;
        }
    }
    return 1; //optimal solution found
}

//function to find the entering col
int find_pivot_col(struct simplexe *s) {
    double max = s->tableau[s->m][0];
    int max_index = 0;

    for (int i=0; i<s->n + s->m; i++) {
        if (s->tableau[s->m][i] > max) {
            max = s->tableau[s->m][i];
            max_index = i;
        }
    }
    return max_index;
}

//function to find the exiting row
int find_pivot_row(struct simplexe *s, int max_index) {
    double min = 1e9;
    int min_index = -1; // if it stays -1 then it not found we have a probleme in our algo
    for (int i=0; i<s->m; i++) {
        if(s->tableau[i][max_index] <= 0) {
            continue;
        } else {
            double ratio = s->tableau[i][s->m+s->n]/s->tableau[i][max_index];
            if (ratio<min) {
                min = ratio;
                min_index = i; 
            }
        }
    }
    return min_index;
}

//pivot function to go to the next iteration
void pivot(struct simplexe *s, int pivotcol, int pivotrow) {
    double pivot = s->tableau [pivotrow][pivotcol];
    for (int i=0 ; i<= s->n + s->m; i++) {
        s->tableau[pivotrow][i] /= pivot; //normalize the pivot row by / on the pivot
    }
    for (int i=0; i <= s->m; i++) {
       if (i == pivotrow) continue;

       double factor = s->tableau[i][pivotcol];
       for (int j=0; j<=s->n+s->m; j++) {
        s->tableau[i][j] -= factor * s->tableau[pivotrow][j]; //updating each entry of the table (square methode)
       }
    }
}

//solve
void solve (struct simplexe *s) {
    int optimal = isoptimal(s);
    while(optimal==0) {
        optimal = isoptimal(s);
        int pivot_col = find_pivot_col(s);
        int pivot_row = find_pivot_row(s, pivot_col);
        if (pivot_row == -1) printf("out of bounds"); //see find_pivot_row function for more details
        pivot(s, pivot_col, pivot_row);
        print_tableau(s);
    } 
    printf("Optimal table found.\n");
}

void print_solution(struct simplexe *s) {
    printf("\nOptimal Solution:\n");
    printf("================\n");
    
    // Array to store the values of original variables
    double solution[MAX_VARS] = {0};
    
    // For each original variable (columns 0 to n-1)
    for (int j = 0; j < s->n; j++) {
        // Check if this column is a basic variable (has exactly one 1 and rest 0s)
        int is_basic = 0;
        int basic_row = -1;
        int count_ones = 0;
        int count_zeros = 0;
        
        // Check the column structure
        for (int i = 0; i <= s->m; i++) {
            if (s->tableau[i][j] == 1.0) {
                count_ones++;
                basic_row = i;
            } else if (s->tableau[i][j] == 0.0) {
                count_zeros++;
            }
        }
        
        // If column has exactly one 1 and rest are 0s, it's a basic variable
        if (count_ones == 1 && count_zeros == s->m) {
            is_basic = 1;
            solution[j] = s->tableau[basic_row][s->n + s->m]; // RHS value
        } else {
            solution[j] = 0.0; // Non-basic variable = 0
        }
    }
    
    // Print the values
    for (int i = 0; i < s->n; i++) {
        printf("x%d = %.2f\n", i + 1, solution[i]);
    }
    
    // Print the optimal objective function value (Z value)
    printf("\nOptimal objective value (Z) = %.2f\n", (s->tableau[s->m][s->n + s->m])*-1);
}


int main () {
    struct simplexe s;
    inputproblem(&s); 
    printf("Tableau N:0");
    print_tableau(&s);
    solve(&s);
    print_solution(&s);
    return 0;
}