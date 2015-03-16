# How to Get MDA Source Code #

Do:
  1. Get Ubuntu
  1. Install some packages
  1. Become a member of our Google Group
  1. Follow Google Code instructions to download the source

Don't:

![http://imgs.xkcd.com/comics/cautionary.png](http://imgs.xkcd.com/comics/cautionary.png)


---


## Get Ubuntu ##
Simply download a disk image from the Ubuntu website and mount it on your USB using http://unetbootin.sourceforge.net/|Unetbootin. <br>
If you want you can also get the Windows Installer (<a href='http://www.ubuntu.com/download/desktop'>http://www.ubuntu.com/download/desktop</a> => Download Windows Installer).<br>
<ul><li>Ubuntu 10 (<a href='http://releases.ubuntu.com/10.10/'>http://releases.ubuntu.com/10.10/</a>) is most similar to the Redhat systems in the ECF labs.<br>
</li><li>Ubuntu 11 and up (<a href='http://releases.ubuntu.com/11.10/'>http://releases.ubuntu.com/11.10/</a>) has a more Mac-like UI.</li></ul>

<h2>Install Packages</h2>

<pre><code>sudo apt-get install build-essential openssh-server subversion freeglut3-dev libncurses5-dev aptitude<br>
sudo apt-get install libcv-dev libcv2.1 libcvaux-dev libcvaux2.1 libhighgui-dev libhighgui2.1<br>
</code></pre>

If you get errors on the second command, try:<br>
<br>
<pre><code>aptitude search libcv<br>
</code></pre>

If you see something like libcv2.3 pop up, then type:<br>
<br>
<pre><code>sudo apt-get install libopencv-dev libcv-dev libcv2.3 libcvaux-dev libcvaux2.3 libhighgui-dev libhighgui2.3<br>
</code></pre>

To be able to compile the technical paper:<br>
<br>
<pre><code>sudo apt-get install texlive-base texlive-fonts-recommended texlive-latex-base texlive-latex-recommended texlive-latex-extra<br>
</code></pre>

<h2>Become a Member</h2>
This should be easy. Email one of the execs at mech.design@utoronto.ca.<br>
<br>
<h2>Download the Code Repository</h2>
Follow the instructions at <a href='http://code.google.com/p/mda-ut/source/checkout'>http://code.google.com/p/mda-ut/source/checkout</a>.<br>
<br>
Specifically, paste this into your terminal<br>
<pre><code>svn checkout https://mda-ut.googlecode.com/svn/trunk/ mda-ut --username MYUSERNAME@gmail.com<br>
</code></pre>
When it asks for your password use the link on the google page to generate your password.