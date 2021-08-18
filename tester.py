import random
import subprocess
import string

intro = "The following script will test the LCS programs using:\n" \
    "* {} tests\n" \
    "* {} as the range of the size for all strings compared\n"\
    "* {} letters from the latin alphabet\n"\
    "* {} processors for the parallel algorithm\n"

strings = []
ran = (50, 200)  # range of size of the strings to be compared
tests = 100  # number of tests to perform
n_letters = 10  # use a subset of #letters from the latin alphabet
processors = 3

print(intro.format(tests, ran, n_letters, processors))

successes = 0

for i in range(0, tests):
    size1 = random.randint(ran[0], ran[1])
    size2 = random.randint(ran[0], ran[1])
    alpha = string.ascii_uppercase
    indices = random.sample(range(0, len(alpha)), k=n_letters)
    letters = [alpha[i] for i in indices]
    string1 = "".join(random.choices(letters, k=size1))
    string2 = "".join(random.choices(letters, k=size2))

    input_file = open("input.txt", "w")
    input_file.write(string1 + "\n" + string2)
    input_file.close()
    result_p = subprocess.run(["mpirun", "-np", str(processors), "./build/lcs_parallel", "./input.txt",
                               "./output_parallel.txt"])
    result_s = subprocess.run(["./build/lcs_sequential", "./input.txt", "./output_sequential.txt"])

    seq_file = open("output_sequential.txt", "r")
    par_file = open("output_parallel.txt", "r")

    out_seq = seq_file.readline().rstrip('\n')
    out_par = par_file.readline().rstrip('\n')

    seq_file.close(), par_file.close()

    if out_seq == out_par:
        print("TEST SUCCEEDED on sequences:\n" + string1 + "\n" + string2 + "\n")
        successes += 1
    else:
        print("TEST FAILED on sequences:\n" + string1 + "\n" + string2 + "\n\n")


print("SUCCESS RATE: " + str(successes) + "/" + str(tests) + "\n")
