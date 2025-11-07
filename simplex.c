#include <stdio.h>

#define MAX_SLACK 10
#define MAX_VARS 10
#define big_M 10000.0

typedef enum { false = 0, true = 1 } bool;

struct simplexe {
  int m;      // number of constraints
  int n;      // number of variables (slack ones not included)
  int is_min; // 0 for max 1 for min
  double tableau[MAX_SLACK + 1]
                [MAX_VARS + MAX_SLACK + MAX_SLACK + 1]; // evident
};

// function to enter fill the table
void inputproblem(struct simplexe *s) {

  printf("Is it a minimizing problem? (0 for max 1 for min) \n");
  scanf("%d", &s->is_min);
  // les variables d'écart ?
  printf("Enter the number of constraints: ");
  scanf("%d", &s->m);
  // donc c'est (x1, x2 etc)
  printf("Enter the number of original variables: ");
  scanf("%d", &s->n);

  // Determine RHS column position
  int rhs_col;
  if (s->is_min == 0) {
    rhs_col = s->n + s->m; // original + slack
  } else {
    rhs_col = s->n + s->m + s->m; // original + surplus + artificial
  }
  // NOTE: why?.

  // Initialize tableau to 0
  for (int i = 0; i <= s->m; i++) {
    for (int j = 0; j <= rhs_col; j++) {
      s->tableau[i][j] = 0;
    }
  }

  // Enter objective function coefficients
  printf("Enter the coefficients of the target (equation économique): ");
  for (int i = 0; i < s->n; i++) {
    scanf("%lf", &s->tableau[s->m][i]);
  }

  // For minimization, add Big M coefficients for artificial variables
  if (s->is_min == 1) {
    for (int i = 0; i < s->m; i++) {
      s->tableau[s->m][s->n + s->m + i] = big_M;
    }
  }

  // Enter constraints
  printf("Enter the coefficients of the constraints (end with RHS): ");
  for (int i = 0; i < s->m; i++) {
    printf("Constraint %d: ", i + 1);
    for (int j = 0; j < s->n; j++) {
      scanf("%lf", &s->tableau[i][j]);
    }
    // RHS at appropriate column
    scanf("%lf", &s->tableau[i][rhs_col]);
  }

  if (s->is_min == 0) {
    // Maximization: add slack variables
    for (int i = 0; i < s->m; i++) {
      s->tableau[i][s->n + i] = 1.0;
    }
  } else {
    // Minimization: add surplus variables
    for (int i = 0; i < s->m; i++) {
      s->tableau[i][s->n + i] = -1.0;
    }
    // Add artificial variables
    for (int i = 0; i < s->m; i++) {
      s->tableau[i][s->n + s->m + i] = 1.0;
    }
  }

  // For minimization with Big M: eliminate artificial variables from objective
  if (s->is_min == 1) {
    for (int j = 0; j <= rhs_col; j++) {
      for (int i = 0; i < s->m; i++) {
        s->tableau[s->m][j] -= s->tableau[i][j] * big_M;
      }
    }
  }
}

// function to print it
void print_tableau(struct simplexe *s) {
  int rhs_col = (s->is_min == 0) ? (s->n + s->m) : (s->n + s->m + s->m);

  printf("\n");
  for (int i = 0; i <= s->m; i++) {
    if (i == s->m) {
      printf(
          "|--------------------------------------------------------------|\n");
    }
    for (int j = 0; j <= rhs_col; j++) {
      if (j == rhs_col) {
        printf(" | ");
      }
      printf("   %.2f   ", s->tableau[i][j]);
    }
    printf("\n");
  }
}

int isoptimal(struct simplexe *s) {
  int total_vars = (s->is_min == 0) ? (s->n + s->m) : (s->n + s->m + s->m);

  if (s->is_min == 0) {
    // For maximization: check if all coefficients are <= 0
    for (int i = 0; i < total_vars; i++) {
      if (s->tableau[s->m][i] > 0) {
        return 0; // not optimal
      }
    }
  } else {
    // For minimization: check if all coefficients are >= 0
    for (int i = 0; i < total_vars; i++) {
      if (s->tableau[s->m][i] < 0) {
        return 0; // not optimal
      }
    }
  }
  return 1; // optimal solution found
}

// function to find the entering col
int find_pivot_col(struct simplexe *s) {
  int total_vars = (s->is_min == 0) ? (s->n + s->m) : (s->n + s->m + s->m);

  if (s->is_min == 0) {
    // For maximization: find most positive
    double max = s->tableau[s->m][0];
    int max_index = 0;

    for (int i = 0; i < total_vars; i++) {
      if (s->tableau[s->m][i] > max) {
        max = s->tableau[s->m][i];
        max_index = i;
      }
    }
    return max_index;
  } else {
    // For minimization: find most negative
    double min = s->tableau[s->m][0];
    int min_index = 0;

    for (int i = 0; i < total_vars; i++) {
      if (s->tableau[s->m][i] < min) {
        min = s->tableau[s->m][i];
        min_index = i;
      }
    }
    return min_index;
  }
}

// function to find the exiting row
int find_pivot_row(struct simplexe *s, int max_index) {
  int rhs_col = (s->is_min == 0) ? (s->n + s->m) : (s->n + s->m + s->m);

  double min = 1e9;
  int min_index =
      -1; // if it stays -1 then not found, we have a problem in our algo
  for (int i = 0; i < s->m; i++) {
    if (s->tableau[i][max_index] <= 0) {
      continue;
    } else {
      double ratio = s->tableau[i][rhs_col] / s->tableau[i][max_index];
      if (ratio < min) {
        min = ratio;
        min_index = i;
      }
    }
  }
  return min_index;
}

// pivot function to go to the next iteration
void pivot(struct simplexe *s, int pivotcol, int pivotrow) {
  int rhs_col = (s->is_min == 0) ? (s->n + s->m) : (s->n + s->m + s->m);

  double pivot = s->tableau[pivotrow][pivotcol];
  for (int i = 0; i <= rhs_col; i++) {
    s->tableau[pivotrow][i] /= pivot; // normalize the pivot row
  }
  for (int i = 0; i <= s->m; i++) {
    if (i == pivotrow)
      continue;

    double factor = s->tableau[i][pivotcol];
    for (int j = 0; j <= rhs_col; j++) {
      s->tableau[i][j] -= factor * s->tableau[pivotrow][j];
    }
  }
}

// solve
void solve(struct simplexe *s) {
  while (isoptimal(s) == 0) {
    int pivot_col = find_pivot_col(s);
    int pivot_row = find_pivot_row(s, pivot_col);
    if (pivot_row == -1) {
      printf("Unbounded solution or error!\n");
      return;
    }
    pivot(s, pivot_col, pivot_row);
    print_tableau(s);
  }
  printf("Optimal table found.\n");
}

void print_solution(struct simplexe *s) {
  int rhs_col = (s->is_min == 0) ? (s->n + s->m) : (s->n + s->m + s->m);

  printf("\nOptimal Solution:\n");
  printf("================\n");

  // Array to store the values of original variables
  double solution[MAX_VARS] = {0};

  // For each original variable (columns 0 to n-1)
  for (int j = 0; j < s->n; j++) {
    // Check if this column is a basic variable (has exactly one 1 and rest 0s)
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
      solution[j] = s->tableau[basic_row][rhs_col]; // RHS value
    } else {
      solution[j] = 0.0; // Non-basic variable = 0
    }
  }

  // Print the values
  for (int i = 0; i < s->n; i++) {
    printf("x%d = %.2f\n", i + 1, solution[i]);
  }

  // Print the optimal objective function value (Z value)
  printf("\nOptimal objective value (Z) = %.2f\n",
         (s->tableau[s->m][rhs_col]) * -1);
}

int main() {
  struct simplexe s;
  inputproblem(&s);
  printf("Tableau N:0");
  print_tableau(&s);
  solve(&s);
  print_solution(&s);
  getchar();
  getchar();
  return 0;
}
