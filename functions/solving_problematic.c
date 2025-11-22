#include "struct.c"

void inputproblem(struct simplexe *s) {

  printf("Is it a minimizing problem? (0 for max 1 for min)\n");
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

// function to print the problematic , or the solution given.
void print_tableau(struct simplexe *s) {
  int rhs_col = (s->is_min == 0) ? (s->n + s->m) : (s->n + s->m + s->m);

  printf("\n");
  printf("+");
  for (int j = 0; j <= rhs_col; j++) {
    printf("-----------+");
    if (j == rhs_col - 1)
      printf("---+");
  }
  printf("\n");

  // Print header row with column numbers
  printf("|");
  for (int j = 0; j <= rhs_col; j++) {
    if (j == rhs_col) {
      printf("   RHS    |");
    } else {
      printf("   x%-3d   |", j + 1);
    }
  }
  printf("\n");

  printf("+");
  for (int j = 0; j <= rhs_col; j++) {
    printf("-----------+");
    if (j == rhs_col - 1)
      printf("---+");
  }
  printf("\n");

  for (int i = 0; i <= s->m; i++) {
    printf("|");

    // Print row label
    if (i < s->m) {
      printf(" C%-3d |", i + 1);
    } else {
      printf(" Obj  |");
    }

    // Print values
    for (int j = 0; j <= rhs_col; j++) {
      if (j == rhs_col) {
        printf(" %8.2f |", s->tableau[i][j]);
      } else {
        printf(" %8.2f |", s->tableau[i][j]);
      }
    }
    printf("\n");

    // Print separator line
    if (i == s->m - 1) {
      printf("+");
      for (int j = 0; j <= rhs_col; j++) {
        printf("-----------+");
        if (j == rhs_col - 1)
          printf("---+");
      }
      printf("\n");
    }
  }

  // Print bottom border
  printf("+");
  for (int j = 0; j <= rhs_col; j++) {
    printf("-----------+");
    if (j == rhs_col - 1)
      printf("---+");
  }
  printf("\n");
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
