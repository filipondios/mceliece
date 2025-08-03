extern int test_matrices(void);
extern int test_mceliece(void);

int main(void) {
    // Run the tests and return number of errors
    return test_matrices() + test_mceliece(); 
}
