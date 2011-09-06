#!/usr/bin/python

import sys, os

from PIL import Image
import getopt

def print_usage(appname):
    print "Usage:\n  %s\n  -i|--input=inputfile <file> -o|--output=outputfile <file>" % appname
    print "  -f|--format=outputformat -O|--overwrite -q|--quality=<0-100>"
    print "  -x|--scalex <value> -y|--scaley <value> -p|-scalep <1-100>"
    print "  -D|--dontkeepratio -h|--help"

def do_scaling(image, scalex, scaley, scalep, keepratio):
    #
    # scale by percentace is overriding factor even if scalex and scaley are defined
    #
    size = image.size
    ratio = float(size[0])/float(size[1])
    if scalep != -1:
        new_x = size[0] * scalep / 100
        new_y = size[1] * scalep / 100
    else:
        if scalex == -1 and scaley == -1: return image
        if scalex == -1: scalex = scaley/ratio
        if scaley == -1: scaley = scalex/ratio
        new_x = scalex
        new_y = scaley
        new_ratio = float(scalex)/float(scaley)
        if keepratio == True:
            if ratio != new_ratio:
                # Keep X, but force change to Y
                new_y = new_x / ratio
    print "resizing from (%d,%d) -> (%d,%d)" % (size[0], size[1], new_x, new_y)
    return image.resize((int(new_x), int(new_y)), Image.BICUBIC)

if __name__ == "__main__": # if run standalone
    try:
        opts, args = getopt.getopt(sys.argv[1:], "i:o:f:Oq:x:y:p:hD", ["input=",
                                                                       "output=",
                                                                       "format=",
                                                                       "overwrite",
                                                                       "quality=",
                                                                       "scalex=",
                                                                       "scaley=",
                                                                       "scalep=",
                                                                       "dontkeepratio",
                                                                       "help"])
    except getopt.GetoptError, err:
        # print help information and exit:
        print str(err)
        print_usage(sys.argv[0])
        sys.exit(2)

    inputfile = ""
    outputfile = ""
    format = ""
    overwrite = False
    qual = 95
    scalex = -1
    scaley = -1
    scalep = -1
    keepratio = True

    for o, a in opts:
        if o in ("-i", "--input"):
            inputfile = str(a)
        elif o in ("-o", "--output"):
            outputfile = str(a)
        elif o in ("-f", "--format"):
            format = str(a)
            format = format.lower()
        elif o in ("-O", "--overwrite"):
            overwrite = True
        elif o in ("-q", "--quality"):
            qual = int(a)
            if qual < 1: qual = 1
            if qual > 95: qual = 95
        elif o in ("-x", "--scalex"):
            scalex = int(a)
            if scalex < 1: scalex = 1
        elif o in ("-y", "--scaley"):
            scaley = int(a)
            if scaley < 1: scaley = 1
        elif o in ("-p", "--scalep"):
            scalep = int(a)
            if scalep < 1: scalep = 1
        elif o in ("-D", "--dontkeepratio"):
            keepratio = False
        elif o in ("-h", "--help"):
            print_usage(sys.argv[0])
            sys.exit(0)
        else:
            assert False, "unhandled option"

    if inputfile == "": print "No input file given. Abort!"; print_usage(sys.argv[0]); sys.exit(2)
    if format == "": format = inputfile[-3:0]
    if outputfile == "": outputfile = inputfile + "." + format
    if os.path.exists(outputfile):
        if overwrite == False:
            print "Outputfile `%s` exists. Aborting. Use --overwrite to force overwriting." % outputfile
            sys.exit(2)
        else:
            print "Removing existing outputfile `%s`" % outputfile
            os.remove(outputfile)
    
    #print "input '%s', output '%s', format '%s', overwrite '%s', quality=%d" % (inputfile, outputfile, format, overwrite, qual)
    #print "scalex %d, scaley %d, scalep %d, keepdatio=%s" % (scalex, scaley, scalep, keepratio)
   
    #
    # file open with PIL
    #
    try: image = Image.open(inputfile)
    except IOError:
        print "PIL: failed to open file `%s`. Abort!" % inputfile
        sys.exit(2)
    pil_format = format
    if format == "jpg": pil_format = "jpeg"


    #
    # Do required manipulations
    #
    image = do_scaling(image, scalex, scaley, scalep, keepratio)


    #
    # Finally save the manipulated image
    #
    try:
        if pil_format != "jpeg":
            image.save(outputfile, pil_format)
        else:
            # Quality affects only JPEG format
            print ("image.save (%s, %s, %d)" % (outputfile, pil_format, qual))
            image.save(outputfile, pil_format, quality=qual)
    except IOError:
        print "PIL: Image save operation failed! I/O Error."

    print "PIL: Output written into `%s`" % outputfile
    print "Done!"
