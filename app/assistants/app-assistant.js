UpstartService.identifier = 'palm://org.webosinternals.upstartmgr';

function AppAssistant() {}

AppAssistant.prototype.handleLaunch = function(params)
{
	try
	{
		if (!params.action) 
		{
			// show banner
			Mojo.Controller.getAppController().showBanner
			(
				{
					icon: 'icon.png',
					messageText: Mojo.appInfo.title + ': Starting...',
					soundClass: ''
				}, 
				{
					action: 'startBannerTap'
				},
				'startBanner'
			);
			// call service to start program
	    	UpstartService.start(this.handleStart.bindAsEventListener(this), Mojo.appInfo.id);
		}
		else
		{
			// we have an action
			// for now the only action is to tap the start banner
			// we're ignoring that for now
		}
	}
	catch (e)
	{
		Mojo.Log.logException(e, "AppAssistant#handleLaunch");
	}
}

AppAssistant.prototype.handleStart = function(payload)
{
	for (p in payload) alert(p + ': ' + payload[p]);
	
	Mojo.Controller.appController.removeBanner('startBanner');
}


function UpstartService() {}

UpstartService.start = function(callback, id)
{
	var request = new Mojo.Service.Request
	(
		UpstartService.identifier,
		{
			method: 'start',
			parameters:
			{
				'id': id
			},
			onSuccess: callback,
			onFailure: callback
		}
	);
	return request;
}