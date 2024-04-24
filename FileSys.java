// FileSys.java Part 1
// @author : Georgette Greborio

// Imports
import java.util.ArrayList;
import java.util.Scanner;


// Main class
public class FileSys {

	// Main class
	public static void main(String[] args) {

		// Create root
		Directory root = new Directory("/");
		Directory current = root;

		// Create a loop to keep asking for cmd-line arguments until program quit with boolean
		boolean exit = false;
		Scanner scanner = new Scanner(System.in); // Read cmd-line arguments

		while (exit == false) { 
			// Print prompt and take keyboard input
			System.out.println("Enter command: ");
			String cmdline = scanner.nextLine();

			// Split command line for all functions
			String[] command = cmdline.split("\\s+");
			String cmd = command[0];

			// This if-loop will sort the different commands and call methods and print results.
			if (cmd.startsWith("create")) {
				
				String fileName = command[1];

				// Error Catch: looking for identical file names.
				if (SystemMethods.findIdentical(fileName, current) != null) {
					System.err.println("ERROR: There is already another file named (" + fileName + "). Please try again.");
				
				} else {
					System.out.println("Enter file contents: ");
					String content = scanner.nextLine();

					File newFile = new File(fileName, content);
		
					current.addFile(newFile);
					System.out.println("File created (" + newFile.nameF + ") and added to current directory (" + current.nameD + ")");

				}
	
			} else if (cmd.startsWith("cat")) {

				String fileName = command[1];
				File fileFound = SystemMethods.findIdentical(fileName, root);
				if (fileFound != null) {
					System.out.println("Printing " + fileName + ": " + fileFound.getContent());
				} else {
					System.err.println("ERROR: File not found. Please enter a valid file name.");
				}


			} else if (cmd.equals("rm")) {
				String file = command[1];
				File fileToRem = SystemMethods.findFile(file, current);
				if (fileToRem != null){
					current.files.remove(fileToRem);
					System.out.println("\nFile removed: ");
					SystemMethods.ls(current);
				} else {
					System.err.println("ERROR: File not found. Please enter a valid file name.");
				}
			
			} else if (cmd.startsWith("mkdir")) {

				String name = command[1];
			
				if (SystemMethods.findDirectory(name, current) != null) {
					System.err.println("ERROR: Directory with this name (" + name + ") already exists. Please try again.");
				} else {
					Directory newDir = new Directory(name);
					current.addDir(newDir);
					System.out.println("New directory created (" + newDir.nameD + ") and added to current directory (" + current.nameD + ")");		
				}

			} else if (cmd.startsWith("rmdir")) {
				String dir = command[1];
				Directory dirToRem = SystemMethods.findSubDir(dir, current);
				current.subdirectories.remove(dirToRem);
				System.out.println("Directory removed."); // + ls
				// EDIT: check for root directory error (with parent)

			} else if (cmd.startsWith("cd")) {

				if (command.length > 1) { // Error catch
			       	if (command[1].equals("/")) {
					    current = root;
					    System.out.println("New current directory: " + current.nameD);
					} else if (!command[1].contains("/")) {
						current = SystemMethods.changeDirectory(command[1], current, root);
					} else if (command[1].contains("/")) {
			            String[] path = command[1].split("/");
			            for (int i = 0; i < path.length; i++) {
			                current = SystemMethods.changeDirectory(path[i], current, root);
			                if (current == null) {
			                    break;
			                }
			            }
			            System.out.println("New current directory: " + current.nameD);

			        } else if (command[1].equals("..")) {
			            if (current.parent != null) {
			                current = current.parent;
			        		System.out.println("New current directory: " + current.nameD);

			            } else {
			                System.out.println("Already at the root directory.");
			            }
			        }
			    } else {
			        System.err.println("Invalid command."); // Error catch
			    }
		
			} else if (cmd.startsWith("ls")) {
				System.out.println("***** Printing directory (" + current.nameD + "):");
				SystemMethods.ls(current);

			} else if (cmd.startsWith("du")) {

				int totalCharacterCount = SystemMethods.du(root);
				System.out.println("Disk Usage (bytes): " + totalCharacterCount);
				// EDIT: error catch for if there are no files created.

			} else if (cmd.startsWith("pwd")) {
				
				String currentPath = SystemMethods.pwd(current);
				System.out.println("Current Directory Path: " + currentPath);

			} else if (cmd.startsWith("find")) {
				SystemMethods.find(command[1], root);
			} else if (cmd.startsWith("exit")) {
				exit = true;
			} else {
				System.err.println("ERROR: Please enter a valid command.");
			}
		}

	}// End main class

}



// Class File: this would be the 'children'
class File{
	public String nameF;
	private String content;

	// Constructor
	File(String n, String c){
		nameF = n;
		content = c;
	}

	// Getters
	public String getContent() { return content; }

} // End class File


//Class Directory: this would be the (non-child) 'nodes'
class Directory {
	public String nameD;
	protected ArrayList<File> files = new ArrayList<>(); // No need to be private, since content is private
    protected ArrayList<Directory> subdirectories = new ArrayList<>();
    protected Directory parent;

	//Constructor
	Directory(String n) {
		this.nameD = n;
        this.subdirectories = new ArrayList<>();
        this.files = new ArrayList<>();
        this.parent = null; // root has no parent
        // EDIT: change (this.) method

	}
	

	void addDir(Directory dir) {
		subdirectories.add(dir);
		dir.parent = this;
	}

	void addFile(File file) {
		files.add(file);
	}

	
} // End class Directory



class SystemMethods { // Class System Methods

	public static File findFile(String name, Directory current) {
	    if (current == null) {
	        System.err.println("ERROR: Current directory is null.");
	        return null;
	    }

	    for (File file : current.files) {
	        if (file.nameF.equals(name)) {
	            return file;
	        }
	    }

	    System.err.println("File not found in the current directory: " + current.nameD);
	    return null;
	}

	public static Directory findDirectory(String name, Directory current) {
	    if (current == null) {
	        System.err.println("ERROR: Current directory is null.");
	        return null;
	    }

	    for (Directory dir : current.subdirectories) {
	        if (dir.nameD.equals(name)) {
	            return dir;
	        }
	    }

	    return null;
	}

	// This method is the same as findFile only it does not print line 205, which was an issue for some uses.
	public static File findIdentical(String name, Directory current) {
	    if (current == null) {
	        System.err.println("ERROR: Current directory is null.");
	        return null;
	    }

	    for (File file : current.files) {
	        if (file.nameF.equals(name)) {
	            return file;
	        }
	    }

	    return null;
	}

	public static Directory findSubDir(String name, Directory current) {
		try {
			for (Directory dir : current.subdirectories) {
				boolean found = false;
				if (dir.nameD.equals(name)) {
					found = true;
					return dir;
				}				
			}

			System.err.println("Directory name does not match any in current directory.");
			return null;

		} catch (NullPointerException i) {
			System.err.print("No subdirectories in this directory: " + i);
			return null;
		}
	}

	public static Directory changeDirectory(String name, Directory current, Directory root) {
	
        if (name.equals("..")) {
	        if (current.parent != null) {
	            current = current.parent;
	            return current;
	        } else {
	            System.out.println("Already at the root directory.");
	            return null;
	        }
	    } else {
	        Directory newDir = findSubDir(name, current);
	        if (newDir == null) {
	            System.err.println("Directory not found.");
	        }
	        return newDir;
	    }
	}
	

	public static void ls(Directory current) {

		if (current.subdirectories.isEmpty() && current.files.isEmpty()) {
			System.out.println("Current directory is empty.");
			return;
		} 

		if (current.subdirectories.isEmpty() && current.files.isEmpty()) {
	        System.out.println("Current directory is empty.");
	    } else {
			for (File file : current.files) {
				System.out.println(file.nameF);
			}
			for (Directory sub : current.subdirectories) {
				System.out.println(sub.nameD + " (*)");
			}
		}
		
	}

	public static int du(Directory current) {
        int totBytes = 0;

        // Call counting method for all files
        for (File file : current.files) {
            totBytes += duHelp(file.getContent());
        }

        // Now doing this in every directory (recursion)
        for (Directory subDir : current.subdirectories) {
            totBytes += du(subDir);
        }

        return totBytes;
    }

    // Byte counting
    public static int duHelp(String content) {
        int byteNum = content.length();
        return byteNum;
    }
   	

   	public static String pwd(Directory current) {
        if (current.parent != null) {
            String path = pwd(current.parent) + "/" + current.nameD;
            return path;
        }

        return current.nameD;
    }

    public static void find(String name, Directory current) {
        findHelper(name, current, pwd(current));
    }

    private static void findHelper(String name, Directory current, String currentPath) {
        // Check files in the current directory
        for (File file : current.files) {
            if (file.nameF.equals(name)) {
                System.out.println("Found file: " + currentPath + "/" + file.nameF);
            }
        }

        // Check subdirectories in the current directory
        for (Directory subDir : current.subdirectories) {
            findHelper(name, subDir, currentPath + "/" + subDir.nameD);
        }
    }
} // End class System Methods





