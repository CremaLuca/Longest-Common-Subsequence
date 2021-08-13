import subprocess

print("The following script will test the LCS programs")



result_p = subprocess.run(["mpirun", "-np", "3", "./build/lcs_parallel"], stdout=subprocess.PIPE)
output_p = result_p.stdout.decode("utf-8")

result_s = subprocess.run(["./build/lcs_sequential"], stdout=subprocess.PIPE)
output_s = result_s.stdout.decode("utf-8")

print(output_p)
print(output_s)