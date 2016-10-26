FROM centos:centos7

# name of the maintainer of this image
MAINTAINER laetitia.guillot@univ-rennes1.fr
USER root
# install the latest upgrades
RUN yum -y update && yum clean all
RUN yum -y install csh
# install dos2unix et git
RUN yum -y install git git-core git-daemon-run
RUN yum -y install dos2unix

RUN git clone https://github.com/laeticlo/Ectoline.git EctoLine
RUN dos2unix /EctoLine/thirdparty/pepnovo/pepnovo.3.1b/Models/*.*
RUN dos2unix /EctoLine/thirdparty/pepnovo/pepnovo.3.1b/Models/*/*.*

ENV PATH /EctoLine/ports/x386-linux/bin:$PATH
ENV PATH /EctoLine/scripts:$PATH

