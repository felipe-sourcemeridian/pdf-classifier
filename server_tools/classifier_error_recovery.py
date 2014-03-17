#!/usr/bin/python
import os
import time
import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.application import MIMEApplication
from subprocess import Popen,PIPE
from subprocess import call
TIME_TO_LIST_PROCESS_AGAIN = 2
USER_NAME = "carlos@linkapedia.com"
PASSWORD = "carlosvi442"
TO_USER = "andresvie@gmail.com"
FORMAT_SUBJECT = "error report %s"
SERVER = "smtp.gmail.com:587"
def main():
	while True:
		if(is_classifier_running()):
			time.sleep(TIME_TO_LIST_PROCESS_AGAIN)
			continue
		try:
			start_classifier()
			send_dump_file_to_email()
		except:
			continue

def start_classifier():
	process_list = Popen(["classifier", "start"], shell=False, stdout=PIPE)
	time.sleep(TIME_TO_LIST_PROCESS_AGAIN)

def send_dump_file_to_email():
	compress_core_dump_and_delete()
	email_server = smtplib.SMTP(SERVER)
	email_server.starttls()
	email_server.login(USER_NAME, PASSWORD)
	msg = get_multipart_message()
	email_server.sendmail(USER_NAME, TO_USER, msg.as_string())

def get_multipart_message():
	msg = MIMEMultipart()
	msg["Subject"] = FORMAT_SUBJECT % (time.asctime())
	msg["From"] = USER_NAME
	msg["To"] = TO_USER
	fp = open("cores.tar.gz", "rb")
	data = fp.read()
	fp.close()
	part = MIMEApplication(data, "gz", name="cores.tar.gz")
	part.add_header("Content-Disposition", "attachment", filename="cores.tar.gz")
	msg.attach(part)
	return msg

	
def compress_core_dump_and_delete():
	cores_zip = Popen(["tar","-cvzf", "cores.tar.gz", "/root/cores"], shell=False, stdout=PIPE)
	cores_zip = Popen(["rm","-rf", "/root/cores/*"], shell=False, stdout=PIPE)
def is_classifier_running():
	process_list = Popen(["ps", "aux"], shell=False, stdout=PIPE)
	process_list.stdout.readline()
	for line in process_list.stdout:
		if "classifierd" in line:
			return True
	return False

if __name__ == '__main__':
	main()
