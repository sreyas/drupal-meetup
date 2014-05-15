Introduction
============
Meetup module allow user can login with Meetup account.
Existing and new users can sign in with Meetup module.
An existing user with an already configured Meetup account 
can log in automaticallyby clicking on the Sign in with 
Meetup button.An existing user signs in with Meetup and 
then logs in into his Drupal user account. This results in the Meetup account 
getting related to the user account so next time Meetup sign in will work.


Installation
============

Meetup php client library   is required for all requests to the Meetup API .
Download the Meetup php client library from , 
https://github.com/blobaugh/Meetup-API-client-for-PHP/
tree/804a5e9b0b82b1697321d5e57b7e4674d3aff025,
Upload zip file into /sites/all/libraries
Then you can extract and rename as meetup_login
First you can add the redierct url in meetup site.
Redirect url be baseurl/meetup/oath;
go to admin/config/services/meetup and follow instructions in order to provide
your Meetup consumer key and consumer secret key and redirect uri .
