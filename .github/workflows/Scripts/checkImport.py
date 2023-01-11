
import os
import re
import sys


def main():
    path="Classes/AppsFlyer/libAppsFlyer"
    filesToCheck = ["AppsFlyerLib.h", "AppsFlyerShareInviteHelper.h"]
    for fileName in filesToCheck:
        file = os.path.join(path, fileName)
        print("Checking the file", file)
        with open(file) as myfile:
            if not re.search("#import \"AppsFlyer.*\.h\"", myfile.read()):
                print("❌ Missing #import with quotes")
                sys.exit(5)
            myfile.seek(0)
            if re.search("#import <AppsFlyer.*", myfile.read()):
                print("❌ AppsFlyer import with bracket in the file")
                sys.exit(5)
            print("✅ imports checked !")
            myfile.close()
            


if __name__ == "__main__":
    main()