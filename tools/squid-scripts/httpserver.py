import BaseHTTPServer
import sys, os
import urllib

from PIL import Image
import getopt

class Handler(BaseHTTPServer.BaseHTTPRequestHandler):

    def logMessage(self, message):
        print message

    def parseURL(self, URL):
        """ self.parseURL(URL): This method will split the incoming URL into parameters
            which will be used later in the proxy code. After calling this method, internal
            class params:
                - URL
                - asset
                - params (LOD, profile)
            are filled and can be used accordingly afterwards.
        """
        self.logMessage("Incoming URL: "+str(URL))
        try:
            self.baseurl, self.asset = URL.split("?", 1)
            try: self.asset, self.params = self.asset.split("&", 1)
            except ValueError: self.params = None
        except ValueError: self.asset = None

        self.logMessage("URL: '"+self.baseurl+"'")
        self.logMessage("asset: '"+str(self.asset)+"'")
        self.logMessage("params: '"+str(self.params)+"'")

        if self.asset == None or self.params == None:
            return

        for i in self.params.split("&"):
            self.logMessage("Processing param: "+str(i))
            try:
                p, v = i.split("=")
                try: v = int(v) # Force translation into integer
                except ValueError: self.logMessage("Param decoding failed. Ignoring!"); continue
            except ValueError: self.logMessage("Param decoding failed. Ignoring!"); continue

            if p.lower() == "lod":
                self.p_LOD = v
                self.logMessage("Detected param '"+str(p)+"' with value '"+str(v))
            elif p.lower() == "profile":
                self.p_Profile = v
                self.logMessage("Detected param '"+str(p)+"' with value '"+str(v))
            else:
                self.logMessage("Errorneous param '"+str(p)+"'. Ignoring!")

    def pushData(self, localfile, mimetype):
        try: f = open(localfile)
        except IOError:
            self.send_response(404)
            self.logMessage("pushData() file open failed! Sending 404")
            return
        self.send_response(200)
        self.send_header("Content-type", mimetype)
        self.end_headers()
        self.wfile.write(f.read())
        f.close()
        os.remove(localfile)
        self.logMessage("data push successful with mimetype: "+str(mimetype))

    def handleImageAndResponse(self, localfile, imagetype):
        try: image = Image.open(localfile)
        except IOError:
            self.send_response(404)
            self.logMessage("PIL image loading failed")
            return
        image.thumbnail((4, 4), Image.ANTIALIAS)
        image.save(localfile, imagetype)
        self.pushData(localfile, "image/"+imagetype)

    def handleMeshAndResponse(self, localfile, meshtype):
        self.pushData(localfile, "model/mesh")
        return

    def do_GET(self):
        """ do_GET(): custom handler for HTTP GET method
            In the proxy, it is assumed for the translation that the
            incoming asset URL will come in following format:

            http://proxyname/?originalassetserver/path/to/asset.png&param1=X&param2=Y
            |----baseurl----||---------------asset----------------||-----params-----|

            Which is being decoded into baseurl, asset reference and params, as shown.

            Supported parameters are:

            - LOD: A number ranging from 1-5 stating the dynamic LOD level which must be
              passed to client. Default LOD is 1, meaning the lower possible translation quality
            - Profile: A name of the

            Note: Squid URLManger passes all relevant URLs to this script. It will handle the
            decision, which URLs shall be translated. All traffic which reaches this point of
            process, shall be translated.
        """
        self.baseurl = ""
        self.asset = ""
        self.params = ""
        self.p_LOD = 1
        self.p_Profile = 1

        self.logMessage("---")
        self.logMessage("Incoming URL: '"+self.path)
        self.parseURL(self.path)

        if self.asset == None: return

        self.logMessage("Retrieving remote file: "+str(self.asset))
        local_filename, headers = urllib.urlretrieve("http://" + self.asset)
        self.logMessage("Downloaded into: "+str(local_filename))
        if os.path.exists(local_filename) == False:
            self.send_response(404)
            self.logMessage("Downloading of the asset failed")
            return

        # At the moment jpg, png, gif and tga file formats are passed
        # from squid url mangler. In this code we shall distinguish just between
        # image and mesh assets, and act accordingly.

        for t in ["jpg", "jpeg", "png", "gif", "tga"]:
            if self.asset.lower().endswith(t):
                if t == "jpg": t = "jpeg" # This is to force PIL to detect Jpeg image format
                self.logMessage("Imagetype "+str(t)+" detected.")
                self.handleImageAndResponse(local_filename, t)
                return

        # Same applies for the meshes. At the moment the code below supports only
        # Ogre meshes, but will be extended to other formats as well.

        for t in ["mesh"]:
            if self.asset.lower().endswith(t):
                self.logMessage("Meshtype "+str(t)+" detected.")
                self.handleMeshAndResponse(local_filename, t)


if __name__ == "__main__": # if run standalone
    try:
        opts, args = getopt.getopt(sys.argv[1:], "uh", ["urlparsertest", "httpserver"])
    except getopt.GetoptError, err:
        # print help information and exit:
        print str(err)
        print "Usage: httpserver.py [-u|urlparsertest] [-h|httpserver]"
        sys.exit(2)

    for o, a in opts:
        if o in ("-u", "--urlparsertest"):
            testurls = [ "http://proxyserver.fi?chiru.cie.fi/path/to/asset.png&LOD=1&Profile=2&Lala=5",
                         "http://proxyserver.fi?chiru.cie.fi/path/to/asset.tga&LOD=1&Profile=2&Lala=5&extra=2",
                         "http://proxyserver.fi?chiru.cie.fi/path/to/asset.jpg",
                         "http://proxyserver.fi?chiru.cie.fi/path/to/asset.JPEG&LOiD=1",
                         "http://proxyserver.fi?chiru.cie.fi/path/to/asset.png&pRoFiLe=56",
                         "http://proxyserver.fi?chiru.cie.fi/path/to/asset.png&LOd=2&profile=ldlld" ]
            h = Handler()
            for i in testurls:
                h.parseURL(i)
            sys.exit()
        elif o in ("-h", "--httpserver"):
            PORT = 8000
            httpd = BaseHTTPServer.HTTPServer(("", PORT), Handler)
            print "serving at port", PORT
            httpd.serve_forever()
        else:
            assert False, "unhandled option"

    print "Done."



