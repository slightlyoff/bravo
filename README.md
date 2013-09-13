# Bravo

Nothing to see here. Move along.

## Still Here?

Fine. Suppose you want to know how to build, run, and contribute eh?

7 Easy steps (that might take a long time to do):

 1. Grab a [chromium checkout](http://dev.chromium.org/developers/how-tos/get-the-code). Some find that the [git submodule flow is faster](https://code.google.com/p/chromium/wiki/UsingGitSubmodules), but any method should work.
 1. `cd src && git clone https://github.com/slightlyoff/bravo`
 1. git apply --check bravo/chromium.patch # if clean, proceed, else ping for help
 1. `git apply bravo/chromium.patch`
 1. `git runhooks` or `build/chromium_gyp` to include Bravo in the build
 1. Build content shell or chrome, e.g. `ninja -C out/Release content_shell`
 1. Drive your freshly minted build to any bravo content you happen to have/find

## Contributing

Patches are only accepted from [Chromium Contributors](http://dev.chromium.org/developers/contributing-code). If you're not yet a Chromium Contributor, please submit a [CLA](https://developers.google.com/open-source/cla/individual?csw=1) or [CCLA](https://developers.google.com/open-source/cla/corporate?csw=1) and let us know about it with PR's.

Code health guidelines are straightforward: this is experimental, but to the greatest extent possible, we use Chromium style. When in doubt, ask.

## Design and Architecture

Bravo, conceptually, is the lowest level of the web platform. To a web developer, that means that everything that's a "bravo API" is something that can't be reasonable self-hosted in JavaScript and which they shouldn't ever expect to be pluggable.

From an implementation perspective, bravo defines the interfaces to JavaScript for system-provided services. We are defining these APIs as we go and bootstrapping on top of the [Pepper C API](https://developers.google.com/native-client/pepperc/) in the Chromium prototype.

Eventually all Bravo APIs (which need not resemble Pepper APIs) will be candidates for standardization. The runtime language -- JavaScript -- is already a web standard.

Libraries built on top of Bravo may implement other standards (such as HTML, DOM, etc.) or provide alternative forms which reduce legacy baggage.

## Bravo Content

To write Bravo content, it must be served with `Content-Type: application/bravo`. The `exmaples/` directory includes a [`.htaccess` file which shows how to enable this in Apache](https://github.com/slightlyoff/bravo/blob/master/examples/.htaccess).

The simplest thing that can possibly invoke bravo is `data:application/bravo,main(b){b.log('oh hai')}`.

All Bravo APIs are plumbed through the `main` function of the script. Also, all bravo scripts run in strict mode by default.