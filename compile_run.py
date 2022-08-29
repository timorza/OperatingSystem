import sys
import subprocess

paramters = []

def main():
	#If we have new files to add
	if len(sys.argv) > 1:
		get_paramters()
		
		#Printing the paramters
		print("Entered: ")
		for par in paramters:
			print(par)

		#creating the sh file using the paramters
		assemble_sh_file()

	print("Running program...")
	run_program()



def assemble_sh_file():
	previous_linking_data =  open("run_os.sh", 'r').read().split("#LINKING")[1]
	previous_compile_data = open("run_os.sh", 'r').read().split("#COMPILING")[1]
	

	#Checking if any paramter is in the file allready
	for par in paramters:
		if par in previous_compile_data or par in previous_linking_data:
			print(par + " Allready in the file, please enter paramters again\n")
			return None



	file = open("run_os.sh","r+")#Rewrite mode
	final_content = ""#Content of the file at the end

	#add the path to the compiler
	final_content += 'export PATH=$PATH:/usr/local/i386elfgcc/bin\n\n\n#COMPILING'
	final_content += previous_compile_data

	for par in paramters:
		if "cpp" in par:
			final_content += 'i386-elf-gcc -ffreestanding -m32 -g -c "'+ par +'" -o "Binaries/'+ par.split('.cpp')[0] +'.o"\n'
		elif "asm" in par:
			final_content += 'nasm "'+ par +'" -f bin -o "Binaries/'+ par.split('.asm')[0] +'.bin"\n'

	final_content += '\n#COMPILING\n\n'

	#now linking command
	final_content += '#LINKING'
	final_content += previous_linking_data.split('--oformat binary')[0]
	
	for par in paramters:
		if "cpp" in par:
			final_content += '"Binaries/' + par.split('.cpp')[0] + '.o" '
	final_content += '--oformat binary\n#LINKING\n\n\n'

	#last lines
	final_content += 'cat "Binaries/bootloader.bin" "Binaries/full_kernel.bin" "Binaries/padding.bin"  > "Binaries/OS.bin"\nqemu-system-x86_64 -drive format=raw,file="Binaries/OS.bin",index=0,if=floppy,  -m 128M'
	file.write(final_content)
	file.close()





def get_paramters():
	for paramter in sys.argv:
		if (not ("py" in paramter)):
			paramters.append(paramter)



def run_program():
	subprocess.call(['sh','./run_os.sh'])


if __name__ == "__main__":
	main()