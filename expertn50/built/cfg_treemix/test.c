IncrSt = ((self->plant[i].Increment - self->plant[i].DeadWdLoss * self->plant[i].CWdWeight/1000.0)/  self->plant[i].density) * (1.0-self->plant[i].UgWdFr)*(1.0-self->plant[i].BrWdFr);	
