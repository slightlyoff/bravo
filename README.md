# Bravo

Nothing to see here. Move along.

## Still Here?

Fine. Suppose you want to know how to build, run, and contribute eh?

7 Easy steps (that might take a long time to do):

 1. Grab a [chromium checkout](http://dev.chromium.org/developers/how-tos/get-the-code). Some find that the [git submodule flow is faster](https://code.google.com/p/chromium/wiki/UsingGitSubmodules), but any method should work.
 1. `cd src && git clone https://github.com/slightlyoff/bravo`
 1. `git apply --check bravo/chromium.patch` # if clean, proceed, else ping for help
 1. `git apply bravo/chromium.patch`
 1. `git runhooks` or `build/chromium_gyp` to include Bravo in the build
 1. Build the Content Shell , e.g. `ninja -C out/Release content_shell`
 1. Drive your freshly minted build to any bravo content you happen to have/find, e.g. on OS X:
 <pre>out/Release/Content\ Shell.app/Contents/MacOS/Content\ Shell  \
   --enable-logging --v=1 \
   http://example.com/test.bravo</pre>

## Contributing

Patches are only accepted from [Chromium Contributors](http://dev.chromium.org/developers/contributing-code). If you're not yet a Chromium Contributor, please submit a [CLA](https://developers.google.com/open-source/cla/individual?csw=1) or [CCLA](https://developers.google.com/open-source/cla/corporate?csw=1) and let us know about it with PR's.

Code health guidelines are straightforward: this is experimental, but to the greatest extent possible, we use Chromium style. When in doubt, ask.

## Design and Architecture

Bravo, conceptually, is the lowest level of the web platform. To a web developer, that means that everything that's a "bravo API" is something that can't be reasonable self-hosted in JavaScript and which they shouldn't ever expect to be pluggable.

From an implementation perspective, bravo defines the interfaces to JavaScript for system-provided services. We are defining these APIs as we go and bootstrapping on top of the [Pepper C API](https://developers.google.com/native-client/pepperc/) in the Chromium prototype.

Eventually all Bravo APIs (which need not resemble Pepper APIs) will be candidates for standardization. The runtime language -- JavaScript -- is already a web standard.

Libraries built on top of Bravo may implement other standards (such as HTML, DOM, etc.) or provide alternative forms which reduce legacy baggage.

## Bravo Content

To write Bravo content, it must be served with `Content-Type: application/bravo`. The `examples/` directory includes a [`.htaccess` file which shows how to enable this in Apache](https://github.com/slightlyoff/bravo/blob/master/examples/.htaccess).

The simplest thing that can possibly invoke bravo is `data:application/bravo,main(b){b.log('oh hai')}`.

All Bravo APIs are plumbed through the `main` function of the script. Also, all bravo scripts run in strict mode by default.

## Using Bravo In Chrome

Bravo, as a result of it's current patch-in mechanism for building with Chrome, isn't as tightly integrated as one might otherwise expect. It _does_ integrate directly with the Content Shell application, but since integrating with Chrome would be a more audacious patch (and therefore more brittle), we've omitted it for now. Luckily you can still run Bravo content in full-blown Chrome if you launch it the right way.

To do this, first build Chrome (or use [Canary](https://www.google.com/intl/en/chrome/browser/canary.html)). Make sure you have also built the `content_shell` target (which will build and copy the right Bravo files to the right places). Next, start Chrome from the command line and pass in the ``--register-pepper-plugins="..."` flag. Here's how it might look on OS X:

<pre>out/Release/Chromium.app/Contents/MacOS/Chromium \
  --register-pepper-plugins="out/Release/ppGoogleBravoPluginChrome.plugin/Contents/MacOS/ppGoogleBravoPluginChrome;application/bravo"</pre>

And on Linux (much terser):

<pre>out/Release/chromium \
  --register-pepper-plugins="out/Release/plugins/libppGoogleBravoPluginChrome.so;application/bravo"</pre>

## Debugging Bravo

The simplest way to debug Bravo is to launch it via Chromium and attach GDB to to the plugin process. E.g., on OS X:

<pre>out/Debug/Chromium.app/Contents/MacOS/Chromium \
  --ppapi-startup-dialog \
  --no-sandbox \
  --register-pepper-plugins="out/Debug/ppGoogleBravoPluginChrome.plugin/Contents/MacOS/ppGoogleBravoPluginChrome;application/bravo"</pre>

The `--ppapi-startup-dialog` ensures that starting the plugin will send a message to stdout when navigating to Bravo content like:

`[...:ERROR:child_process.cc(130)] Ppapi (29489) paused waiting for debugger to attach. Send SIGUSR1 to unpause.`

From there, simply attach GDB to the PID listed in the message (in this case 29489) to continue. Huzzah.